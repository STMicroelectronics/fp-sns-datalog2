/**
  ******************************************************************************
  * @file    VD6283.c
  * @author  IMG SW Application Team
  * @brief   Driver File
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __KERNEL__
#include <stdlib.h>
#include <string.h>
#else
#include <linux/string.h>
#include <linux/bitops.h>
#include <linux/math64.h>
#include <linux/bug.h>
#endif

#include "STALS.h"
#include "STALS_platform.h"
#include "VD6283_map.h"

#include "STALS_compat.h"

#ifndef VD6283_CONFIG_DEVICES_MAX
#define VD6283_CONFIG_DEVICES_MAX		1
#endif

#define UID_LEN					16

#define VD6283_CHANNEL_NB			STALS_ALS_MAX_CHANNELS

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)				(sizeof(a) / sizeof(a[0]))
#endif

#define VD6283_DC_CHANNELS_MASK			0x1f
#define VD6283_AC_CHANNELS_MASK			0x20

#define VD6283_DEFAULT_HF_TRIM			0x0e3
#define VD6283_DEFAULT_LF_TRIM			0x07
#define VD6283_DEFAULT_FILTER_CONFIG		2
#define VD6283_DEFAULT_GAIN			0x80

#define VD6283_DEFAULT_OTP_VERSION	0x15

#ifndef MAX
#define MAX(a, b)				((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)				((a) < (b) ? (a) : (b))
#endif

#define VD6283_DEFAULT_FILTER_INDEX		2

static const uint16_t GainRange[] = {
	0x42AB,		/*   66, 67 */
	0x3200,		/*   50, 00 */
	0x2154,		/*   33, 33 */
	0x1900,		/*   25, 00 */
	0x10AB,		/*   16, 67 */
	0x0A00,		/*   10, 00 */
	0x0723,		/*    7, 14 */
	0x0500,		/*    5, 00 */
	0x0354,		/*    3, 33 */
	0x0280,		/*    2, 50 */
	0x01AB,		/*    1, 67 */
	0x0140,		/*    1, 25 */
	0x0100,		/*    1, 00 */
	0x00D4,		/*    0, 83 */
	0x00B5		/*    0, 71 */
};

static const uint16_t GainRangeThreshold[] = {
	0x3A56,
	0x29AB,
	0x1D2B,
	0x14D6,
	0x0D56,
	0x0892,
	0x0612,
	0x042B,
	0x02EB,
	0x0216,
	0x0176,
	0x0121,
	0x00EB,
	0x00C5
};

enum dev_state {
	DEV_FREE = 0,
	DEV_INIT,
	DEV_ALS_RUN,
	DEV_FLICKER_RUN,
	DEV_BOTH_RUN
};

struct VD6283_device {
	enum dev_state st;
	enum STALS_Mode_t als_started_mode;
	void *client;
	void *hdl;
	uint8_t device_id;
	uint8_t revision_id;
	uint8_t dc_chan_en;
	uint8_t ac_chan_en;
	struct {
		uint8_t chan;
	} als;
	struct {
		uint8_t chan;
	} flk;
	uint16_t gains[VD6283_CHANNEL_NB];
	uint32_t exposure;
	enum STALS_FlickerOutputType_t flicker_output_type;
	enum STALS_Control_t is_otp_usage_enable;
	enum STALS_Control_t is_output_dark_enable;
	uint64_t otp_bits[2];
	struct {
		uint16_t hf_trim;
		uint8_t lf_trim;
		uint8_t filter_config;
		uint8_t filter_index;
		uint8_t gains[STALS_ALS_MAX_CHANNELS];
		uint8_t otp_version;
	} otp;
	char uid[UID_LEN];
};

static struct VD6283_device devices[VD6283_CONFIG_DEVICES_MAX];

static const enum STALS_Color_Id_t
channel_2_color_rgb_ir_clear_ir_cut_visible[VD6283_CHANNEL_NB] = {
	STALS_COLOR_RED, STALS_COLOR_CLEAR_IR_CUT, STALS_COLOR_BLUE,
	STALS_COLOR_GREEN, STALS_COLOR_IR, STALS_COLOR_CLEAR
};

#define CHECK_DEVICE_VALID(d) \
do { \
	if (!d) \
		return STALS_ERROR_INVALID_DEVICE_ID; \
} while (0)

#define CHECK_NULL_PTR(a) \
do { \
	if (!a) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_VALID(idx) \
do { \
	if ((idx) >= VD6283_CHANNEL_NB) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECH_DEV_ST_INIT(d) \
do { \
	if (d->st != DEV_INIT) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_DEV_ST_FLICKER_STARTED(d) \
do { \
	if (d->st != DEV_FLICKER_RUN && d->st != DEV_BOTH_RUN) \
		return STALS_ERROR_NOT_STARTED; \
} while (0)

#define CHECH_DEV_ST_FLICKER_NOT_STARTED(d) \
do { \
	if (d->st == DEV_FLICKER_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
	if (d->st == DEV_BOTH_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_DEV_ST_ALS_STARTED(d) \
do { \
	if (d->st != DEV_ALS_RUN && d->st != DEV_BOTH_RUN) \
		return STALS_ERROR_NOT_STARTED; \
} while (0)

#define CHECH_DEV_ST_ALS_NOT_STARTED(d) \
do { \
	if (d->st == DEV_ALS_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
	if (d->st == DEV_BOTH_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_CHANNEL_MASK_VALID_FOR_ALS(m) \
do { \
	/* check there is no invalid channel selected */  \
	if ((m) & ~((1 << STALS_ALS_MAX_CHANNELS) - 1)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_MASK_VALID(m) \
do { \
	/* check there is no invalid channel selected */  \
	if ((m) & ~((1 << STALS_ALS_MAX_CHANNELS) - 1)) \
		return STALS_ERROR_INVALID_PARAMS; \
	/* check there is at least one channel */ \
	if (!(m)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_NOT_IN_USE(d, c) \
do { \
	if (d->als.chan & (c)) \
		return STALS_ERROR_INVALID_PARAMS; \
	if (d->flk.chan & (c)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_FILTER_MASK_VALID(d, m) \
do { \
	if (!is_filter_mask_valid(d,m)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)


/* Fw declarations */
static STALS_ErrCode_t set_channel_gain(struct VD6283_device *dev, int c,
	uint16_t Gain);
static STALS_ErrCode_t set_pedestal_value(struct VD6283_device *dev,
	uint32_t value);

static inline int channelId_2_index(enum STALS_Channel_Id_t ChannelId)
{
	return POPCOUNT(ChannelId) == 1 ? CTZ(ChannelId) :
		STALS_ALS_MAX_CHANNELS;
}

static int is_filter_mask_valid(struct VD6283_device *UNUSED_P(dev), uint8_t Channels)
{
	int c;

	for (c = 0; c < VD6283_CHANNEL_NB; c++) {
		if (!(Channels & (1 << c)))
			continue;
		if (!(channel_2_color_rgb_ir_clear_ir_cut_visible[c] != STALS_COLOR_INVALID))
			return 0;
	}

	return 1;
}

static int is_cut_2_3(struct VD6283_device *dev)
{
	return (dev->device_id == VD6283_DEVICE) &&
		(dev->revision_id == VD6283_REVISION);
}

static int is_cut_valid(struct VD6283_device *dev)
{
	return is_cut_2_3(dev);
}

static struct VD6283_device *get_device(void **pHandle)
{
	uint32_t i;

	for (i = 0; i < VD6283_CONFIG_DEVICES_MAX; i++) {
		if (!devices[i].st)
			break;
	}
	*pHandle = (void *)(uintptr_t)i;

	return i == VD6283_CONFIG_DEVICES_MAX ? NULL : &devices[i];
}

static void setup_device(struct VD6283_device *dev, void *pClient, void *hdl)
{
	memset(dev, 0, sizeof(*dev));
	dev->client = pClient;
	dev->hdl = hdl;
	dev->st = DEV_INIT;
	dev->is_otp_usage_enable = STALS_CONTROL_ENABLE;
	dev->is_output_dark_enable = STALS_CONTROL_DISABLE;
	dev->exposure = 80000;
}

static STALS_ErrCode_t otp_read_bank(struct VD6283_device *dev, int bank,
	int bit_start, int bit_end, uint32_t *opt)
{
	int bit_nb = bit_end - bit_start + 1;
	uint32_t bit_mask = (uint32_t) ((1 << bit_nb) - 1);

	*opt = (uint32_t)((dev->otp_bits[bank] >> bit_start) & bit_mask);

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t otp_read(struct VD6283_device *dev, int bit_start,
	int bit_nb, uint32_t *otp, int bit_swap)
{
	STALS_ErrCode_t res;
	const int bit_end = bit_start + bit_nb - 1;
	uint32_t bank0 = 0;
	uint32_t bank1 = 0;
	uint32_t result;
	uint32_t result_swap = 0;
	int bank1_shift = 0;
	int i;

	if (bit_nb > 24 || bit_end >= 120) {
			return STALS_ERROR_INVALID_PARAMS;
	}

	if (bit_start < 64) {
		res = otp_read_bank(dev, 0, bit_start, MIN(63, bit_end),
			&bank0);
		if (res)
			return res;
		bank1_shift = MIN(63, bit_end) - bit_start + 1;
	}
	if (bit_end >= 64) {
		res = otp_read_bank(dev, 1, MAX(0, bit_start - 64),
			bit_end - 64, &bank1);
		if (res)
			return res;
	}

	result = bank0 | (bank1 << bank1_shift);
	if (bit_swap) {
		result_swap = 0;
		for (i = 0; i < bit_nb; i++) {
			if ((result >> i) & 1)
				result_swap |= (uint32_t)(1 << (bit_nb - 1 - i));
		}
	}
	*otp = (uint32_t)(bit_swap ? result_swap : result);

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t check_supported_device(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	res = STALS_RdByte(dev->client, VD6283_DEVICE_ID, &dev->device_id);
	if (res)
		return res;
	res = STALS_RdByte(dev->client, VD6283_REVISION_ID, &dev->revision_id);
	if (res)
		return res;

	if (is_cut_valid(dev))
		return STALS_NO_ERROR;

	return STALS_ERROR_INVALID_DEVICE_ID;
}

static STALS_ErrCode_t is_data_ready(struct VD6283_device *dev,
	uint8_t *is_data_ready)
{
	STALS_ErrCode_t res;
	uint8_t isr_ctrl_st;

	res = STALS_RdByte(dev->client, VD6283_IRQ_CTRL_ST, &isr_ctrl_st);
	*is_data_ready = !(isr_ctrl_st & 0x02);

	return res;
}

static STALS_ErrCode_t ac_mode_update(struct VD6283_device *dev, uint8_t mask,
	uint8_t data)
{
	STALS_ErrCode_t res;
	uint8_t ac_mode;

	res = STALS_RdByte(dev->client, VD6283_AC_MODE, &ac_mode);
	if (res)
		return res;
	ac_mode = (uint8_t)((ac_mode & ~mask) | (data & mask));
	return STALS_WrByte(dev->client, VD6283_AC_MODE, ac_mode);
}

static STALS_ErrCode_t acknowledge_irq(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(dev->client, VD6283_IRQ_CTRL_ST, 1);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_IRQ_CTRL_ST, 0);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t dev_sw_reset(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(dev->client, VD6283_GLOBAL_RESET, 1);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_GLOBAL_RESET, 0);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t otp_reset(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint8_t status;
	int max_loop_nb = 100;

	res = STALS_WrByte(dev->client, VD6283_OTP_CTRL1, 0);
	if (res)
		return res;

	do {
		res = STALS_RdByte(dev->client, VD6283_OTP_STATUS, &status);
		if (res)
			return res;
	} while ((status & VD6283_OTP_DATA_READY) !=
		 VD6283_OTP_DATA_READY && --max_loop_nb);
	if (!max_loop_nb)
		return STALS_ERROR_TIME_OUT;

	return STALS_NO_ERROR;
}

static uint8_t byte_bit_reversal(uint8_t d)
{
	d = (uint8_t) (((d & 0xaa) >> 1) | ((d & 0x55) << 1));
	d = (uint8_t) (((d & 0xcc) >> 2) | ((d & 0x33) << 2));
	d = (uint8_t) (((d & 0xf0) >> 4) | ((d & 0x0f) << 4));

	return d;
}

static STALS_ErrCode_t opt_read_init(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint8_t reg[8];
	int i;

	dev->otp_bits[0] = 0;

	res = STALS_RdMultipleBytes(dev->client, VD6283_OTP_BANK_0, reg, 8);
	if (res)
		return res;

	for (i = 0; i < 7; i++) {
		dev->otp_bits[0] |=
			(uint64_t) byte_bit_reversal(reg[i]) << (i * 8);
	}
	reg[7] = byte_bit_reversal(reg[7]) >> 4;
	dev->otp_bits[0] |=  (uint64_t) reg[7] << 56;

	res = STALS_RdMultipleBytes(dev->client, VD6283_OTP_BANK_1, reg, 8);
	if (res)
		return res;

	reg[0] = byte_bit_reversal(reg[0]);
	dev->otp_bits[0] |=  (uint64_t) (reg[0] & 0x0f) << 60;
	dev->otp_bits[1] = reg[0] >> 4;

	for (i = 1; i < 7; i++) {
		dev->otp_bits[1] |=
			(uint64_t) byte_bit_reversal(reg[i]) << (i * 8 - 4);
	}
	reg[7] = byte_bit_reversal(reg[7]) >> 4;
	dev->otp_bits[1] |=  (uint64_t) reg[7] << 52;

	return STALS_NO_ERROR;
}

static int fixup_otp_filter_config(struct VD6283_device *dev)
{
	uint32_t check[3];
	int res;

	res = otp_read(dev, 80, 16, &check[0], 0);
	if (res)
		return 0;
	res = otp_read(dev, 96, 14, &check[1], 0);
	if (res)
		return 0;
	res = otp_read(dev, 75, 5,  &check[2], 1);
	if (res)
		return 0;

	if (check[0] != 0x56c1)
		return 0;
	if (check[1] != 0x0364)
		return 0;

	if (check[2] < 1 || check[2] > 6)
		return 0;

	return 1;
}

static STALS_ErrCode_t otp_read_param_details(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	int i;

	res = otp_read(dev, 51, 9, &otp_data, 1);
	if (res)
		goto error;
	dev->otp.hf_trim = (uint16_t) otp_data;
	res = otp_read(dev, 116, 4, &otp_data, 1);
	if (res)
		goto error;
	dev->otp.lf_trim = (uint8_t) otp_data;
	res = otp_read(dev, 48, 3, &otp_data, 1);
	if (res)
		goto error;
	dev->otp.filter_config =(uint8_t) otp_data;
	for (i = 0; i < STALS_ALS_MAX_CHANNELS; i++) {
		res = otp_read(dev, i * 8, 8, &otp_data, 1);
		if (res)
			goto error;
		/* 0 is not a valid value */
		dev->otp.gains[i] =(uint8_t) (otp_data ? otp_data : VD6283_DEFAULT_GAIN);
	}

	if (fixup_otp_filter_config(dev))
		dev->otp.filter_config = 0x03;

	return STALS_NO_ERROR;

error:
	return res;
}

static STALS_ErrCode_t otp_read_param(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	int i;

	/* default values in case we don't recognize otp version */
	dev->otp.hf_trim = VD6283_DEFAULT_HF_TRIM;
	dev->otp.lf_trim = VD6283_DEFAULT_LF_TRIM;
	dev->otp.filter_config = VD6283_DEFAULT_FILTER_CONFIG;
	dev->otp.filter_index = VD6283_DEFAULT_FILTER_INDEX;
	dev->otp.otp_version = VD6283_DEFAULT_OTP_VERSION;
	for (i = 0; i < STALS_ALS_MAX_CHANNELS; i++)
		dev->otp.gains[i] = VD6283_DEFAULT_GAIN;

	/* read otp version */
	res = otp_read(dev, 113, 3, &otp_data, 1);
	if (res)
		goto error;
	/* For Rainbow2 extend OTP Version */
	otp_data = otp_data+0x10;
	dev->otp.otp_version = (uint8_t) otp_data;

	res = otp_read_param_details(dev);
	dev->otp.filter_index = VD6283_DEFAULT_FILTER_INDEX;

error:
	return res;
}

static char nible_to_ascii(uint8_t nible)
{
	return nible < 10 ? (char)( '0' + nible) : (char)('a' + nible - 10);
}

static char encode_nible(uint8_t nible, uint8_t *xor_reg)
{
	uint8_t prev_xor_reg = *xor_reg;

	*xor_reg = nible ^ *xor_reg;

	return nible_to_ascii(nible ^ prev_xor_reg);
}

static STALS_ErrCode_t otp_generate_uid(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	uint8_t xor_reg = 0;
	uint8_t nible;
	int index = 0;
	int i;

	/* uid will have 14 chars */
	/* encode first nible with otp version first */
	res = otp_read(dev, 113, 3, &otp_data, 1);
	if (res)
		goto error;
	nible = (uint8_t) (otp_data << 1);
	res = otp_read(dev, 60, 1, &otp_data, 1);
	if (res)
		goto error;
	nible |= (uint8_t) otp_data;
	dev->uid[index++] = encode_nible(nible, &xor_reg);
	/* next nibles follow otp layout */
	for (i = 61; i < 112; i += 4) {
		res = otp_read(dev, i, 4, &otp_data, 1);
		if (res)
			goto error;
		dev->uid[index++] = encode_nible((uint8_t) otp_data, &xor_reg);
	}

	return STALS_NO_ERROR;

error:
	return res;
}

static STALS_ErrCode_t trim_oscillators(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint16_t hf_trim = dev->is_otp_usage_enable ? dev->otp.hf_trim :
		VD6283_DEFAULT_HF_TRIM;
	uint8_t lf_trim = dev->is_otp_usage_enable ? dev->otp.lf_trim :
		VD6283_DEFAULT_LF_TRIM;

	/* 10.24Mhz */
	res = STALS_WrByte(dev->client, VD6283_OSC10M, 1);
	if (res)
		return res;

	res = STALS_WrByte(dev->client, VD6283_OSC10M_TRIM_M,
		(hf_trim >> 8) & 0x01);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_OSC10M_TRIM_L,
		 (uint8_t)((hf_trim >> 0) & 0xff));
	if (res)
		return res;

	/* 48.7 Khz */
	res = STALS_WrByte(dev->client, VD6283_OSC50K_TRIM, lf_trim & 0x0f);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t clamp_enable(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(dev->client, VD6283_AC_CLAMP_EN, 1);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_DC_CLAMP_EN, 0x1f);
	if (res)
		return res;
	/* set count to be independent of pd nummbers */
	res = STALS_WrByte(dev->client, VD6283_SPARE_1, 0x3f);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t select_pd(struct VD6283_device *dev)
{
	uint8_t pds[] = {0x07, 0x07, 0x07, 0x1f, 0x0f, 0x1f};
	STALS_ErrCode_t res;
	int c;

	pds[3] = 0x0d;
	for (c = 0; c < STALS_ALS_MAX_CHANNELS; c++) {
		res = STALS_WrByte(dev->client, (uint8_t) VD6283_SEL_PD_x(c), pds[c]);
		if (res)
			return res;
	}

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t set_dithering(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(dev->client, VD6283_OSC_DITHERING_STEP_VALUE, 1);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_OSC_DITHERING_STEP_DURATION, 1);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_OSC_DITHERING_STEP_NUMBER, 0x09);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_OSC_DITHERING_CTRL, 1);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t dev_configuration(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	int c;

	res = otp_reset(dev);
	if (res)
		return res;

	res = opt_read_init(dev);
	if (res)
		return res;

	res = otp_read_param(dev);
	if (res)
		return res;

	res = otp_generate_uid(dev);
	if (res)
		return res;

	res = trim_oscillators(dev);
	if (res)
		return res;

	res = clamp_enable(dev);
	if (res)
		return res;

	res = select_pd(dev);
	if (res)
		return res;

	for (c = 0; c < STALS_ALS_MAX_CHANNELS; c++) {
		res = set_channel_gain(dev, c, 0x0100);
		if (res)
			return res;
	}

	res = set_pedestal_value(dev, 3);
	if (res)
		return res;

	res = set_dithering(dev);
	if (res)
		return res;

	res = STALS_WrByte(dev->client, VD6283_SPARE_0,
				VD6283_SPARE_0_IN_GPIO2);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static void put_device(struct VD6283_device *dev)
{
	dev_sw_reset(dev);
	/* reset otp to avoid high consumption */
	otp_reset(dev);
	STALS_WrByte(dev->client, VD6283_SPARE_0,
				VD6283_SPARE_0_IN_GPIO2);
	dev->st = DEV_FREE;
}

static struct VD6283_device *get_active_device(void *pHandle)
{
	uint32_t handle = (uint32_t)(uintptr_t) pHandle;

	if (handle >= VD6283_CONFIG_DEVICES_MAX)
		return NULL;
	if (devices[handle].st == DEV_FREE)
		return NULL;

	return &devices[handle];
}

static STALS_ErrCode_t get_channel_gain(struct VD6283_device *dev, int c,
	uint16_t *pAppliedGain)
{
	*pAppliedGain = dev->gains[c];

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t set_channel_gain(struct VD6283_device *dev, int c,
	uint16_t Gain)
{
	STALS_ErrCode_t res;
	uint8_t vref;

	for (vref = ARRAY_SIZE(GainRange) - 1; vref > 0; vref--) {
		if (Gain < GainRangeThreshold[vref - 1])
			break;
	}

	res = STALS_WrByte(dev->client,(uint8_t) VD6283_CHANNEL_VREF(c), (uint8_t)(vref + 1));
	if (res)
		return res;
	dev->gains[c] = GainRange[vref];

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t get_exposure(struct VD6283_device *dev,
	uint32_t *pAppliedExpoTimeUs)
{
	*pAppliedExpoTimeUs = dev->exposure;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t set_exposure(struct VD6283_device *dev,
	uint32_t ExpoTimeInUs, int is_cache_updated)
{
	const uint32_t step_size_us = 1600;
	const uint32_t rounding = step_size_us / 2;
	uint32_t exposure;
	uint64_t exposure_acc;
	STALS_ErrCode_t res;

	/* avoid integer overflow using intermediate 64 bits arithmetics */
	exposure_acc = ExpoTimeInUs + (uint64_t) rounding;
	exposure = (uint32_t) div64_u64(MIN(exposure_acc, 0xffffffffULL), step_size_us);
	exposure = MAX(exposure, 1);
	exposure = MIN(exposure, 0x3ff);
	res = STALS_WrByte(dev->client, VD6283_EXPOSURE_L, exposure & 0xff);
	if (res)
		return res;
	res = STALS_WrByte(dev->client, VD6283_EXPOSURE_M,
		(exposure >> 8) & 0x3);
	if (res)
		return res;

	if (is_cache_updated)
		dev->exposure = exposure * step_size_us;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t dev_enable_channels_for_mode(struct VD6283_device *dev,
	enum STALS_Mode_t mode, uint8_t channels)
{
	uint8_t active_chan = dev->als.chan | dev->flk.chan;
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t dc_chan_en;
	STALS_ErrCode_t res;

	dc_chan_en =(uint8_t) ((channels & VD6283_DC_CHANNELS_MASK) | dev->dc_chan_en);
	if (channels & active_chan)
		return STALS_ERROR_INVALID_PARAMS;

	if (dc_chan_en ^ dev->dc_chan_en) {
		res = STALS_WrByte(dev->client, VD6283_DC_EN, dc_chan_en);
		if (res)
			return res;
	}
	if ((channels & VD6283_AC_CHANNELS_MASK) &&
	    !dev->ac_chan_en) {
		res = STALS_WrByte(dev->client, VD6283_AC_EN, 1);
		if (res) {
			STALS_WrByte(dev->client, VD6283_DC_EN,
				dev->dc_chan_en);
			return res;
		}
		dev->ac_chan_en = 1;
	}

	dev->dc_chan_en = dc_chan_en;

	if (is_flk)
		dev->flk.chan = channels;
	else
		dev->als.chan = channels;

	return STALS_NO_ERROR;
}

static uint8_t dev_disable_dc_chan_en_for_mode(struct VD6283_device *dev,
	enum STALS_Mode_t mode)
{
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t channels = is_flk ? dev->flk.chan : dev->als.chan;
	uint8_t res;

	res =(uint8_t)(dev->dc_chan_en & ~(channels & VD6283_DC_CHANNELS_MASK));

	return res;
}

static STALS_ErrCode_t dev_disable_channels_for_mode(struct VD6283_device *dev,
	enum STALS_Mode_t mode)
{
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t channels = is_flk ? dev->flk.chan : dev->als.chan;
	uint8_t dc_chan_en = dev_disable_dc_chan_en_for_mode(dev, mode);
	STALS_ErrCode_t res;

	if (dc_chan_en ^ dev->dc_chan_en) {
		res = STALS_WrByte(dev->client, VD6283_DC_EN, dc_chan_en);
		if (res)
			return res;
	}

	if (channels & VD6283_AC_CHANNELS_MASK) {
		res = STALS_WrByte(dev->client, VD6283_AC_EN, 0);
		if (res) {
			STALS_WrByte(dev->client, VD6283_DC_EN,
				dev->dc_chan_en);
			return res;
		}
		dev->ac_chan_en = 0;
	}
	dev->dc_chan_en = dc_chan_en;

	if (is_flk)
		dev->flk.chan = 0;
	else
		dev->als.chan = 0;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t enable_flicker_output_mode(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;
	uint8_t pdm_select_output = PDM_SELECT_GPIO1;
	uint8_t pdm_select_clk = PDM_SELECT_INTERNAL_CLK;
	uint8_t intr_config = 0x00;
	uint8_t sel_dig = 0x00;
	uint8_t spare0 = VD6283_SPARE_0_IN_GPIO2;
	enum STALS_FlickerOutputType_t FlickerOutputType =
		dev->flicker_output_type;

	/* STALS_FLICKER_OUTPUT_ANALOG : pdm data on GPIO2 pin and use
	 * internal clock for sigma-delta 1 bit dac.
	 * STALS_FLICKER_OUTPUT_DIGITAL_PDM : pdm data on GPIO1 pin and use
	 * external clock feed from GPIO2 pin to drive sigma-delta 1 bit dac.
	 */
	switch (FlickerOutputType) {
	case STALS_FLICKER_OUTPUT_DIGITAL_PDM:
		pdm_select_clk = PDM_SELECT_EXTERNAL_CLK;
		intr_config = 0x01;
		sel_dig = 0x0f;
		break;
	case STALS_FLICKER_OUTPUT_ANALOG:
		pdm_select_output = PDM_SELECT_GPIO2;
		spare0 = VD6283_SPARE_0_OUT_GPIO2;
		break;
	default:
		return STALS_ERROR_INVALID_PARAMS;
	}
	res = ac_mode_update(dev, PDM_SELECT_OUTPUT | PDM_SELECT_CLK,
		pdm_select_output | pdm_select_clk);
	if (res)
		return res;

	/* interrupt pin output stage configuration */
	res = STALS_WrByte(dev->client, VD6283_INTR_CFG, intr_config);
	if (res)
		return res;

	/* interrupt output selection */
	res = STALS_WrByte(dev->client, VD6283_DTEST_SELECTION, sel_dig);
	if (res)
		return res;

	res = STALS_WrByte(dev->client, VD6283_SPARE_0, spare0);
	if (res)
			return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t disable_flicker_output_mode(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	/* restore spare0 */
	res = STALS_WrByte(dev->client, VD6283_SPARE_0,
				VD6283_SPARE_0_IN_GPIO2);
	if (res)
		return res;

	/* restore ac_mode bit 4 and bit 5 */
	res = ac_mode_update(dev, PDM_SELECT_OUTPUT | PDM_SELECT_CLK,
		PDM_SELECT_GPIO1 | PDM_SELECT_INTERNAL_CLK);
	if (res)
		return res;

	/* restore OD output stage configuration */
	res = STALS_WrByte(dev->client, VD6283_INTR_CFG, 0x00);
	if (res)
		return res;

	/* and output als_complete signal */
	res = STALS_WrByte(dev->client, VD6283_DTEST_SELECTION, 0x00);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t stop_als(struct VD6283_device *dev,
	enum STALS_Mode_t mode)
{
	STALS_ErrCode_t res;

	if (dev->st != DEV_ALS_RUN && dev->st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	if (dev->als_started_mode != mode)
		return STALS_ERROR_NOT_STARTED;

	res = STALS_WrByte(dev->client, VD6283_ALS_CTRL, 0);
	if (res)
		return res;

	res = dev_disable_channels_for_mode(dev, mode);
	if (res)
		return res;

	dev->st = dev->st == DEV_ALS_RUN ? DEV_INIT : DEV_FLICKER_RUN;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t start_als(struct VD6283_device *dev, uint8_t channels,
	enum STALS_Mode_t mode)
{
	STALS_ErrCode_t res=STALS_NO_ERROR;
	uint8_t cmd = (mode == STALS_MODE_ALS_SYNCHRONOUS) ? VD6283_ALS_START |
		VD6283_ALS_CONTINUOUS | VD6283_ALS_CONTINUOUS_SLAVED :
		VD6283_ALS_START;

	if (dev->st == DEV_ALS_RUN || dev->st == DEV_BOTH_RUN)
		return STALS_ERROR_ALREADY_STARTED;

	res = dev_enable_channels_for_mode(dev, mode, channels);
	if (res)
		return res;

	if (dev->st == DEV_FLICKER_RUN) {
		/* stop flicker */
		res = ac_mode_update(dev, AC_EXTRACTOR, AC_EXTRACTOR_DISABLE);
		if (res)
			goto start_als_error;
	}

	res = STALS_WrByte(dev->client, VD6283_ALS_CTRL, cmd);
	if (res)
		goto start_als_error;

	if (dev->st == DEV_FLICKER_RUN) {
		/* restart flicker */
		res = ac_mode_update(dev, AC_EXTRACTOR, AC_EXTRACTOR_ENABLE);
	}

	dev->st = dev->st == DEV_INIT ? DEV_ALS_RUN : DEV_BOTH_RUN;
	dev->als_started_mode = mode;

	return res ;

start_als_error:
	dev_disable_channels_for_mode(dev, mode);

	return res;
}

static STALS_ErrCode_t start_single_shot(struct VD6283_device *dev,
	uint8_t channels)
{
	return start_als(dev, channels, STALS_MODE_ALS_SINGLE_SHOT);
}

static STALS_ErrCode_t start_synchronous(struct VD6283_device *dev,
	uint8_t channels)
{
	return start_als(dev, channels, STALS_MODE_ALS_SYNCHRONOUS);
}

static STALS_ErrCode_t start_flicker(struct VD6283_device *dev,
	uint8_t channels)
{
	STALS_ErrCode_t res;
	uint8_t ac_channel_select;

	/* This check only one channel is selected */
	CHECK_CHANNEL_VALID(channelId_2_index((enum STALS_Channel_Id_t)channels));

	if (dev->st == DEV_FLICKER_RUN || dev->st == DEV_BOTH_RUN)
		return STALS_ERROR_ALREADY_STARTED;

	res = dev_enable_channels_for_mode(dev, STALS_MODE_FLICKER, channels);
	if (res)
		return res;

	res = enable_flicker_output_mode(dev);
	if (res) {
		dev_disable_channels_for_mode(dev, STALS_MODE_FLICKER);
		return res;
	}

	/* now enable ac and select channel ac channel */
	ac_channel_select = channels == (uint8_t) STALS_CHANNEL_6 ?  (uint8_t)1 :
		(uint8_t) (channelId_2_index((enum STALS_Channel_Id_t)channels) + 2);
	res = ac_mode_update(dev, (uint8_t) (AC_CHANNEL_SELECT | AC_EXTRACTOR),
		 (uint8_t) ((ac_channel_select << 1) | (uint8_t) AC_EXTRACTOR_ENABLE));
	if (res) {
		disable_flicker_output_mode(dev);
		dev_disable_channels_for_mode(dev, STALS_MODE_FLICKER);
		return res;
	}

	dev->st = dev->st == DEV_INIT ? DEV_FLICKER_RUN : DEV_BOTH_RUN;

	return res;
}

static STALS_ErrCode_t stop_single_shot(struct VD6283_device *dev)
{
	return stop_als(dev, STALS_MODE_ALS_SINGLE_SHOT);
}

static STALS_ErrCode_t stop_synchronous(struct VD6283_device *dev)
{
	return stop_als(dev, STALS_MODE_ALS_SYNCHRONOUS);
}

static STALS_ErrCode_t stop_flicker(struct VD6283_device *dev)
{
	STALS_ErrCode_t res;

	if (dev->st != DEV_FLICKER_RUN && dev->st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	res = ac_mode_update(dev, AC_EXTRACTOR, AC_EXTRACTOR_DISABLE);
	if (res)
		return res;
	disable_flicker_output_mode(dev);
	dev_disable_channels_for_mode(dev, STALS_MODE_FLICKER);
	dev->st = dev->st == DEV_FLICKER_RUN ? DEV_INIT : DEV_ALS_RUN;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t get_pedestal_enable(struct VD6283_device *dev,
	uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t ac_mode;

	res = STALS_RdByte(dev->client, VD6283_AC_MODE, &ac_mode);
	*value = (ac_mode & AC_PEDESTAL) ? STALS_CONTROL_DISABLE :
		STALS_CONTROL_ENABLE;

	return res;
}

static STALS_ErrCode_t get_pedestal_value(struct VD6283_device *dev,
	uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t data;

	res = STALS_RdByte(dev->client, VD6283_AC_PEDESTAL, &data);
	*value = data & VD6283_PEDESTAL_VALUE_MASK;

	return res;
}

static STALS_ErrCode_t get_otp_usage_enable(struct VD6283_device *dev,
	uint32_t *value)
{
	*value = dev->is_otp_usage_enable;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t get_output_dark_enable(struct VD6283_device *dev,
	uint32_t *value)
{
	*value = dev->is_output_dark_enable;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t get_sda_drive_value(struct VD6283_device *dev,
	uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t data;

	res = STALS_RdByte(dev->client, VD6283_SDA_DRIVE, &data);
	data &= VD6283_SDA_DRIVE_MASK;
	*value =(uint32_t) ((data + 1) * 4);

	return res;
}

static STALS_ErrCode_t get_saturation_value(struct VD6283_device *dev,
	uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t sat_m_before;
	uint8_t sat_m_after;
	uint8_t sat_l;

	do {
		res = STALS_RdByte(dev->client, VD6283_AC_SAT_METRIC_M, &sat_m_before);
		res |= STALS_RdByte(dev->client, VD6283_AC_SAT_METRIC_L, &sat_l);
		res |= STALS_RdByte(dev->client, VD6283_AC_SAT_METRIC_M, &sat_m_after);
		if (res)
			return res;
	} while(sat_m_before != sat_m_after);
	*value = (uint32_t) ((sat_m_before << 8) + sat_l);

	return STALS_NO_ERROR;
}


static STALS_ErrCode_t set_pedestal_enable(struct VD6283_device *dev,
	uint32_t value)
{
	return ac_mode_update(dev, AC_PEDESTAL,
		value ? AC_PEDESTAL_ENABLE : AC_PEDESTAL_DISABLE);
}

static STALS_ErrCode_t set_pedestal_value(struct VD6283_device *dev,
	uint32_t value)
{
	return STALS_WrByte(dev->client, VD6283_AC_PEDESTAL,
		value & VD6283_PEDESTAL_VALUE_MASK);
}

static STALS_ErrCode_t set_otp_usage_enable(struct VD6283_device *dev,
	uint32_t value)
{
	STALS_ErrCode_t res;

	dev->is_otp_usage_enable = (enum STALS_Control_t) value;
	/* oscillator trimming configuration need to be done again */
	res = trim_oscillators(dev);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t set_output_dark_enable(struct VD6283_device *dev,
	uint32_t value)
{
	STALS_ErrCode_t res;
	uint8_t pd_value = value ? 0x18 : 0x07;

	res = STALS_WrByte(dev->client, VD6283_SEL_PD_x(1), pd_value);
	if (res)
		return res;

	dev->is_output_dark_enable = (enum STALS_Control_t) value;

	return STALS_NO_ERROR;
}

static STALS_ErrCode_t set_sda_drive_value(struct VD6283_device *dev,
	uint32_t value)
{
	STALS_ErrCode_t res;
	uint8_t data;
	uint8_t sda_drive_reg_value;

	/* valid values are 4, 8, 12, 16, 20 */
	if (value > 20 || value == 0 || value % 4)
		return STALS_ERROR_INVALID_PARAMS;

	sda_drive_reg_value = (uint8_t) ((value / 4) - 1);
	res = STALS_RdByte(dev->client, VD6283_SDA_DRIVE, &data);
	if (res)
		return res;

	data = (uint8_t) ((data & ~VD6283_SDA_DRIVE_MASK) | sda_drive_reg_value );
	return STALS_WrByte(dev->client, VD6283_SDA_DRIVE, data);
}


 static uint8_t calfactor( uint8_t gain)
 {
 	if (gain&BIT(3))
 		return (uint8_t) (0x80|(gain&0x07)<<2);
 	else
 		return (uint8_t) ((0x20-gain)<<2);
 }

 static uint8_t calfactor1050(uint16_t channelgain, uint8_t gain)
 {

 	if (channelgain < 0x2154) //use cal10
 		return  calfactor((gain&0xF0)>>4);
 	else //use cal50
 		return calfactor(gain&0x0F);
 }

 static void apply_calfactor(struct VD6283_device *dev,
 	struct STALS_Als_t *pAlsValue)
 {
	uint8_t otp_coef_cal;
	uint32_t calibrated_count;
 	int c;

 	for (c = 0; c < VD6283_CHANNEL_NB; c++){
		if (!(dev->als.chan & (1 << c)))
 			continue;
		// get otp_coefcal
		otp_coef_cal = dev->is_otp_usage_enable ? calfactor1050(dev->gains[c], dev->otp.gains[c]) :
				VD6283_DEFAULT_GAIN;

		// apply
		/* note that no overflow can occur */
		calibrated_count =(otp_coef_cal * pAlsValue->CountValueRaw[c]) >>7;
		/* be sure to not generate values greater that saturation level ... */
		calibrated_count = MIN(calibrated_count, 0x00ffffff);
		/* ... but also avoid value below 0x100 except for zero */
		calibrated_count = calibrated_count < 0x100 ? 0 : calibrated_count;
		// store result
		pAlsValue->CountValue[c] = calibrated_count;
 	}
 }


/* public API */
/* STALS_ERROR_INVALID_PARAMS */
STALS_ErrCode_t STALS_Init(char *UNUSED_P(pDeviceName), void *pClient,
	void **pHandle)
{
	STALS_ErrCode_t res;
	struct VD6283_device *dev;

	CHECK_NULL_PTR(pHandle);
	// get an empty device structure associated to handle
	dev = get_device(pHandle);
	if (!dev) {
		// NULL POINTEUR
		res = STALS_ERROR_INIT;
		goto get_device_error;
	}
	// configure dev structure with default values
	setup_device(dev, pClient, *pHandle);
	//i2c read access no otp read
	res = check_supported_device(dev);
	if (res)
		// STALS_ERROR_READ STALS_ERROR_INVALID_DEVICE_ID
		goto check_supported_device_error;
	// first i2c writes
	res = dev_sw_reset(dev);
	if (res)
		// STALS_ERROR_WRITE
		goto dev_sw_reset_error;
	res = dev_configuration(dev);
	if (res)
		// STALS_ERROR_READ STALS_ERROR_WRITE STALS_ERROR_TIME_OUT
		goto dev_configuration_error;
	// now we can check otp version
	if (dev->otp.otp_version != VD6283_DEFAULT_OTP_VERSION) {
		res = STALS_ERROR_INVALID_DEVICE_ID;
		goto check_supported_device_error;
	}
	return STALS_NO_ERROR;

dev_configuration_error:
dev_sw_reset_error:
check_supported_device_error:
	put_device(dev);
get_device_error:
	return res;
}

STALS_ErrCode_t STALS_Term(void *pHandle)
{
	struct VD6283_device *dev = get_active_device(pHandle);

	CHECK_DEVICE_VALID(dev);

	put_device(dev);

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_GetVersion(uint32_t *pVersion, uint32_t *pRevision)
{
	CHECK_NULL_PTR(pVersion);
	CHECK_NULL_PTR(pRevision);

	*pVersion = (VERSION_MAJOR << 16) + VERSION_MINOR;
	*pRevision = VERSION_REVISION;

	return STALS_NO_ERROR;
}

/* NOTE : don't use CHECK_FILTER_MASK_VALID so we can return STALS_COLOR_INVALID
 *        color.
 */
STALS_ErrCode_t STALS_GetChannelColor(void *pHandle,
	enum STALS_Channel_Id_t ChannelId, enum STALS_Color_Id_t *pColor)
{
	struct VD6283_device *dev = get_active_device(pHandle);

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pColor);
	CHECK_CHANNEL_VALID(channelId_2_index(ChannelId));

	*pColor = channel_2_color_rgb_ir_clear_ir_cut_visible[channelId_2_index(ChannelId)];

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_SetExposureTime(void *pHandle, uint32_t ExpoTimeInUs,
	uint32_t *pAppliedExpoTimeUs)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedExpoTimeUs);
	CHECH_DEV_ST_ALS_NOT_STARTED(dev);

	res = set_exposure(dev, ExpoTimeInUs, 1);
	if (res)
		return res;

	res = get_exposure(dev, pAppliedExpoTimeUs);

	return res;
}

STALS_ErrCode_t STALS_GetExposureTime(void *pHandle,
	uint32_t *pAppliedExpoTimeUs)
{
	struct VD6283_device *dev = get_active_device(pHandle);

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedExpoTimeUs);

	return get_exposure(dev, pAppliedExpoTimeUs);
}

STALS_ErrCode_t STALS_SetInterMeasurementTime(void *pHandle,
	uint32_t InterMeasurmentInUs, uint32_t *pAppliedInterMeasurmentInUs)
{
	const uint32_t step_size_us = 20500;
	const uint32_t rounding = step_size_us / 2;
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;
	uint64_t value_acc;
	uint8_t value;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedInterMeasurmentInUs);
	CHECH_DEV_ST_ALS_NOT_STARTED(dev);

	/* avoid integer overflow using intermediate 64 bits arithmetics */
	value_acc = InterMeasurmentInUs + (uint64_t) rounding;
	value_acc = div64_u64(MIN(value_acc, 0xffffffffULL), step_size_us);
	value = (uint8_t) MIN(value_acc, 0xff);
	res = STALS_WrByte(dev->client, VD6283_CONTINUOUS_PERIOD, value);
	if (res)
		return res;

	return STALS_GetInterMeasurementTime(pHandle,
		pAppliedInterMeasurmentInUs);
}

STALS_ErrCode_t STALS_GetInterMeasurementTime(void *pHandle,
	uint32_t *pAppliedInterMeasurmentInUs)
{
	const uint32_t step_size_us = 20500;
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;
	uint8_t value;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedInterMeasurmentInUs);

	res = STALS_RdByte(dev->client, VD6283_CONTINUOUS_PERIOD, &value);
	if (res)
		return res;

	*pAppliedInterMeasurmentInUs = step_size_us * value;

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_GetProductVersion(void *pHandle, uint8_t *pDeviceID,
	uint8_t *pRevisionID)
{
	struct VD6283_device *dev = get_active_device(pHandle);

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pDeviceID);
	CHECK_NULL_PTR(pRevisionID);

	*pDeviceID = dev->device_id;
	*pRevisionID = dev->revision_id;

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_SetGain(void *pHandle, enum STALS_Channel_Id_t ChannelId,
	uint16_t Gain, uint16_t *pAppliedGain)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	int chan = channelId_2_index(ChannelId);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedGain);
	CHECK_CHANNEL_VALID(chan);
	CHECK_FILTER_MASK_VALID(dev, ChannelId);
	CHECK_CHANNEL_NOT_IN_USE(dev, ChannelId);

	res = set_channel_gain(dev, chan, Gain);
	if (res)
		return res;

	res = get_channel_gain(dev, chan, pAppliedGain);

	return res;
}
STALS_ErrCode_t STALS_GetGain(void *pHandle, enum STALS_Channel_Id_t ChannelId,
	uint16_t *pAppliedGain)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	int chan = channelId_2_index(ChannelId);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAppliedGain);
	CHECK_CHANNEL_VALID(chan);
	CHECK_FILTER_MASK_VALID(dev, ChannelId);

	res = get_channel_gain(dev, chan, pAppliedGain);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_SetFlickerOutputType(void *pHandle,
	enum STALS_FlickerOutputType_t FlickerOutputType)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res = STALS_NO_ERROR;

	CHECK_DEVICE_VALID(dev);
	CHECH_DEV_ST_FLICKER_NOT_STARTED(dev);

	switch (FlickerOutputType) {
	case STALS_FLICKER_OUTPUT_DIGITAL_PDM:
		dev->flicker_output_type = FlickerOutputType;
		break;
	case STALS_FLICKER_OUTPUT_ANALOG:
		dev->flicker_output_type = FlickerOutputType;
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

STALS_ErrCode_t STALS_Start(void *pHandle, enum STALS_Mode_t Mode,
	uint8_t Channels)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);
	CHECK_CHANNEL_MASK_VALID(Channels);
	CHECK_FILTER_MASK_VALID(dev, Channels);

	switch (Mode) {
	case STALS_MODE_ALS_SINGLE_SHOT:
		res = start_single_shot(dev, Channels);
		break;
	case STALS_MODE_ALS_SYNCHRONOUS:
		res = start_synchronous(dev, Channels);
		break;
	case STALS_MODE_FLICKER:
		res = start_flicker(dev, Channels);
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

STALS_ErrCode_t STALS_Stop(void *pHandle, enum STALS_Mode_t Mode)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);

	switch (Mode) {
	case STALS_MODE_ALS_SINGLE_SHOT:
		res = stop_single_shot(dev);
		break;
	case STALS_MODE_ALS_SYNCHRONOUS:
		res = stop_synchronous(dev);
		break;
	case STALS_MODE_FLICKER:
		res = stop_flicker(dev);
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

STALS_ErrCode_t STALS_GetAlsValues(void *pHandle, uint8_t Channels,
	struct STALS_Als_t *pAlsValue, uint8_t *pMeasureValid)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res = STALS_NO_ERROR;
	uint8_t c;
	uint32_t rawdata[VD6283_CHANNEL_NB];

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pAlsValue);
	CHECK_NULL_PTR(pMeasureValid);
	CHECK_CHANNEL_MASK_VALID_FOR_ALS(Channels);
	CHECK_FILTER_MASK_VALID(dev, Channels);
	CHECK_DEV_ST_ALS_STARTED(dev);

	if (dev->st != DEV_ALS_RUN && dev->st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	/* do we have ready datas */
	res = is_data_ready(dev, pMeasureValid);
	if (res)
		return res;
	if (!*pMeasureValid)
		return STALS_NO_ERROR;

	Channels &= dev->als.chan;



	res = STALS_RdMultipleBytes(dev->client,
								(uint8_t)VD6283_CHANNELx(0),
								(uint8_t*) rawdata,
								VD6283_CHANNEL_NB * 4 );
	if (res != STALS_NO_ERROR)
		return res;

	for (c = 0; c < VD6283_CHANNEL_NB; c++) {
		if (Channels & (1 << c)){
			pAlsValue->CountValueRaw[c]=  ((rawdata[c] & 0xff) << 24) | ((rawdata[c] & 0xff00) << 8) | ((rawdata[c] & 0xff0000) >> 8);
			pAlsValue->Gains[c] = dev->gains[c];
		}
		else{
			pAlsValue->CountValueRaw[c] = 0;
		}
	}
	pAlsValue->Channels = Channels;
	apply_calfactor(dev, pAlsValue);

	/* acknowledge irq */
	return acknowledge_irq(dev);
}


STALS_ErrCode_t STALS_GetFlickerFrequency(void *pHandle,
	struct STALS_FlickerInfo_t *pFlickerInfo)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pFlickerInfo);
	return STALS_ERROR_FNCT_DEPRECATED;
}

STALS_ErrCode_t STALS_SetControl(void *pHandle,
	enum STALS_Control_Id_t ControlId, uint32_t ControlValue)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);

	switch (ControlId) {
	case STALS_PEDESTAL_ENABLE:
		CHECH_DEV_ST_FLICKER_NOT_STARTED(dev);
		res = set_pedestal_enable(dev, ControlValue);
		break;
	case STALS_PEDESTAL_VALUE:
		CHECH_DEV_ST_FLICKER_NOT_STARTED(dev);
		res = set_pedestal_value(dev, ControlValue);
		break;
	case STALS_OTP_USAGE_ENABLE:
		CHECH_DEV_ST_INIT(dev);
		res = set_otp_usage_enable(dev, ControlValue);
		break;
	case STALS_OUTPUT_DARK_ENABLE:
		CHECH_DEV_ST_INIT(dev);
		res = set_output_dark_enable(dev, ControlValue);
		break;
	case STALS_SDA_DRIVE_VALUE_MA:
		CHECH_DEV_ST_INIT(dev);
		res = set_sda_drive_value(dev, ControlValue);
		break;
	case STALS_WA_STATE:
		res = STALS_ERROR_INVALID_PARAMS;
		break;
	case STALS_ALS_AUTOGAIN_CTRL:
		res =  STALS_ERROR_FNCT_DEPRECATED;
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

STALS_ErrCode_t STALS_GetControl(void *pHandle,
	enum STALS_Control_Id_t ControlId, uint32_t *pControlValue)
{
	struct VD6283_device *dev = get_active_device(pHandle);
	STALS_ErrCode_t res;

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pControlValue);

	switch (ControlId) {
	case STALS_PEDESTAL_ENABLE:
		res = get_pedestal_enable(dev, pControlValue);
		break;
	case STALS_PEDESTAL_VALUE:
		res = get_pedestal_value(dev, pControlValue);
		break;
	case STALS_OTP_USAGE_ENABLE:
		res = get_otp_usage_enable(dev, pControlValue);
		break;
	case STALS_OUTPUT_DARK_ENABLE:
		res = get_output_dark_enable(dev, pControlValue);
		break;
	case STALS_SDA_DRIVE_VALUE_MA:
		res = get_sda_drive_value(dev, pControlValue);
		break;
	case STALS_WA_STATE:
		res = STALS_ERROR_INVALID_PARAMS;
		break;
	case STALS_SATURATION_VALUE:
		res = get_saturation_value(dev, pControlValue);
		break;
	case STALS_ALS_AUTOGAIN_CTRL:
		res =  STALS_ERROR_FNCT_DEPRECATED;
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

STALS_ErrCode_t STALS_GetUid(void *pHandle, char **pUid)
{
	struct VD6283_device *dev = get_active_device(pHandle);

	CHECK_DEVICE_VALID(dev);
	CHECK_NULL_PTR(pUid);

	*pUid = &dev->uid[0];

	return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_RdMultipleBytes(void *pClient, uint8_t index,
	uint8_t *data, int nb) __attribute__((weak));
STALS_ErrCode_t STALS_RdMultipleBytes(void *pClient, uint8_t index,
	uint8_t *data, int nb)
{
	STALS_ErrCode_t res = STALS_NO_ERROR;
	uint8_t i = 0;

	for (i = 0; i < (uint8_t) nb; i++) {
		res = STALS_RdByte(pClient, (uint8_t) (index + i), &data[i]);
		if (res)
			return res;
	}

	return res;
}
