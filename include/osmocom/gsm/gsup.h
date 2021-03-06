/*! \defgroup gsup Generic Subscriber Update Protocol
 *  @{
 *
 *  The Generic Subscriber Update Protocol (GSUP) is an Osmocom-specific
 *  non-standard protocol replacing MAP as the protocol between
 *  MSC/VLR/SGSN and HLR in a 3GPP cellular communications network.
 *
 *  It was designed around the same transactions and architecture as the
 *  MAP messages/operations, but without the complexity of TCAP and MAP,
 *  and without the need for ASN.1 encoding.
 *
 *  The purpose is to keep protocol complexity out of OsmoSGSN and
 *  OsmoMSC, while providing a clean path to an external GSUP to MAP
 *  translator.
 *
 *  \file gsup.h
 *  Osmocom Generic Subscriber Update Protocol message encoder/decoder. */
/*
 * (C) 2014 by sysmocom - s.f.m.c. GmbH, Author: Jacob Erlbeck
 * (C) 2016 by Harald Welte <laforge@gnumonks.org>
 * All Rights Reserved
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include <stdint.h>
#include <osmocom/core/msgb.h>
#include <osmocom/gsm/protocol/gsm_23_003.h>
#include <osmocom/gsm/protocol/gsm_04_08_gprs.h>
#include <osmocom/crypt/auth.h>

#define OSMO_GSUP_PORT 4222

/*! Maximum nubmer of PDP inside \ref osmo_gsup_message */
#define OSMO_GSUP_MAX_NUM_PDP_INFO		10 /* GSM 09.02 limits this to 50 */
/*! Maximum number of auth info inside \ref osmo_gsup_message */
#define OSMO_GSUP_MAX_NUM_AUTH_INFO		5
/*! Maximum number of octets encoding MSISDN in BCD format */
#define OSMO_GSUP_MAX_MSISDN_LEN		9
#define OSMO_GSUP_MAX_CALLED_PARTY_BCD_LEN	43 /* TS 24.008 10.5.4.7 */

#define OSMO_GSUP_PDP_TYPE_SIZE			2

/*! Information Element Identifiers for GSUP IEs */
enum osmo_gsup_iei {
	OSMO_GSUP_IMSI_IE			= 0x01,
	OSMO_GSUP_CAUSE_IE			= 0x02,
	OSMO_GSUP_AUTH_TUPLE_IE			= 0x03,
	OSMO_GSUP_PDP_INFO_COMPL_IE		= 0x04,
	OSMO_GSUP_PDP_INFO_IE			= 0x05,
	OSMO_GSUP_CANCEL_TYPE_IE		= 0x06,
	OSMO_GSUP_FREEZE_PTMSI_IE		= 0x07,
	OSMO_GSUP_MSISDN_IE			= 0x08,
	OSMO_GSUP_HLR_NUMBER_IE			= 0x09,
	OSMO_GSUP_PDP_CONTEXT_ID_IE		= 0x10,
	OSMO_GSUP_PDP_TYPE_IE			= 0x11,
	OSMO_GSUP_ACCESS_POINT_NAME_IE		= 0x12,
	OSMO_GSUP_PDP_QOS_IE			= 0x13,
	OSMO_GSUP_CHARG_CHAR_IE			= 0x14,
	OSMO_GSUP_RAND_IE			= 0x20,
	OSMO_GSUP_SRES_IE			= 0x21,
	OSMO_GSUP_KC_IE				= 0x22,
	/* 3G support */
	OSMO_GSUP_IK_IE				= 0x23,
	OSMO_GSUP_CK_IE				= 0x24,
	OSMO_GSUP_AUTN_IE			= 0x25,
	OSMO_GSUP_AUTS_IE			= 0x26,
	OSMO_GSUP_RES_IE			= 0x27,
	OSMO_GSUP_CN_DOMAIN_IE			= 0x28,

	OSMO_GSUP_SESSION_ID_IE			= 0x30,
	OSMO_GSUP_SESSION_STATE_IE		= 0x31,

	/*! Supplementary Services payload */
	OSMO_GSUP_SS_INFO_IE			= 0x35,
};

/*! GSUP message type */
enum osmo_gsup_message_type {
	OSMO_GSUP_MSGT_UPDATE_LOCATION_REQUEST	= 0b00000100,
	OSMO_GSUP_MSGT_UPDATE_LOCATION_ERROR	= 0b00000101,
	OSMO_GSUP_MSGT_UPDATE_LOCATION_RESULT	= 0b00000110,

	OSMO_GSUP_MSGT_SEND_AUTH_INFO_REQUEST	= 0b00001000,
	OSMO_GSUP_MSGT_SEND_AUTH_INFO_ERROR	= 0b00001001,
	OSMO_GSUP_MSGT_SEND_AUTH_INFO_RESULT	= 0b00001010,

	OSMO_GSUP_MSGT_AUTH_FAIL_REPORT		= 0b00001011,

	OSMO_GSUP_MSGT_PURGE_MS_REQUEST		= 0b00001100,
	OSMO_GSUP_MSGT_PURGE_MS_ERROR		= 0b00001101,
	OSMO_GSUP_MSGT_PURGE_MS_RESULT		= 0b00001110,

	OSMO_GSUP_MSGT_INSERT_DATA_REQUEST	= 0b00010000,
	OSMO_GSUP_MSGT_INSERT_DATA_ERROR	= 0b00010001,
	OSMO_GSUP_MSGT_INSERT_DATA_RESULT	= 0b00010010,

	OSMO_GSUP_MSGT_DELETE_DATA_REQUEST	= 0b00010100,
	OSMO_GSUP_MSGT_DELETE_DATA_ERROR	= 0b00010101,
	OSMO_GSUP_MSGT_DELETE_DATA_RESULT	= 0b00010110,

	OSMO_GSUP_MSGT_LOCATION_CANCEL_REQUEST	= 0b00011100,
	OSMO_GSUP_MSGT_LOCATION_CANCEL_ERROR	= 0b00011101,
	OSMO_GSUP_MSGT_LOCATION_CANCEL_RESULT	= 0b00011110,

	OSMO_GSUP_MSGT_PROC_SS_REQUEST		= 0b00100000,
	OSMO_GSUP_MSGT_PROC_SS_ERROR		= 0b00100001,
	OSMO_GSUP_MSGT_PROC_SS_RESULT		= 0b00100010,
};

#define OSMO_GSUP_IS_MSGT_REQUEST(msgt) (((msgt) & 0b00000011) == 0b00)
#define OSMO_GSUP_IS_MSGT_ERROR(msgt)   (((msgt) & 0b00000011) == 0b01)
#define OSMO_GSUP_TO_MSGT_ERROR(msgt)   (((msgt) & 0b11111100) | 0b01)

extern const struct value_string osmo_gsup_message_type_names[];
static inline const char *
osmo_gsup_message_type_name(enum osmo_gsup_message_type val)
{	return get_value_string(osmo_gsup_message_type_names, val); }

enum osmo_gsup_cancel_type {
	OSMO_GSUP_CANCEL_TYPE_UPDATE		= 1, /* on wire: 0 */
	OSMO_GSUP_CANCEL_TYPE_WITHDRAW		= 2, /* on wire: 1 */
};

enum osmo_gsup_cn_domain {
	OSMO_GSUP_CN_DOMAIN_PS			= 1,
	OSMO_GSUP_CN_DOMAIN_CS			= 2,
};

/*! TCAP-like session state */
enum osmo_gsup_session_state {
	/*! Undefined session state */
	OSMO_GSUP_SESSION_STATE_NONE		= 0x00,
	/*! Initiation of a new session */
	OSMO_GSUP_SESSION_STATE_BEGIN		= 0x01,
	/*! Communication of an existing session */
	OSMO_GSUP_SESSION_STATE_CONTINUE	= 0x02,
	/*! Indication of the session end */
	OSMO_GSUP_SESSION_STATE_END		= 0x03,
};

extern const struct value_string osmo_gsup_session_state_names[];
static inline const char *
osmo_gsup_session_state_name(enum osmo_gsup_session_state val)
{	return get_value_string(osmo_gsup_session_state_names, val); }

/*! parsed/decoded PDP context information */
struct osmo_gsup_pdp_info {
	unsigned int			context_id;
	int				have_info;
	/*! Type of PDP context */
	uint16_t			pdp_type;
	/*! APN information, still in encoded form. Can be NULL if no
	 * APN information included */
	const uint8_t			*apn_enc;
	/*! length (in octets) of apn_enc */
	size_t				apn_enc_len;
	/*! QoS information, still in encoded form. Can be NULL if no
	 * QoS information included */
	const uint8_t			*qos_enc;
	/*! length (in octets) of qos_enc */
	size_t				qos_enc_len;
	/*! PDP Charging Characteristics, still in encoded form. Can be NULL if no
	 * PDP Charging Characteristics */
	const uint8_t			*pdp_charg_enc;
	/*! length (in octets) of pdp_charg_enc */
	size_t				pdp_charg_enc_len;
};

/*! parsed/decoded GSUP protocol message */
struct osmo_gsup_message {
	enum osmo_gsup_message_type	message_type;
	char				imsi[GSM23003_IMSI_MAX_DIGITS+2];
	enum gsm48_gmm_cause		cause;
	enum osmo_gsup_cancel_type	cancel_type;
	int				pdp_info_compl;
	int				freeze_ptmsi;
	struct osmo_auth_vector		auth_vectors[OSMO_GSUP_MAX_NUM_AUTH_INFO];
	size_t				num_auth_vectors;
	struct osmo_gsup_pdp_info	pdp_infos[OSMO_GSUP_MAX_NUM_PDP_INFO];
	size_t				num_pdp_infos;
	const uint8_t			*msisdn_enc;
	size_t				msisdn_enc_len;
	const uint8_t			*hlr_enc;
	size_t				hlr_enc_len;
	const uint8_t			*auts;
	const uint8_t			*rand;
	enum osmo_gsup_cn_domain	cn_domain;
	const uint8_t			*pdp_charg_enc;
	size_t				pdp_charg_enc_len;

	/*! Session state \ref osmo_gsup_session_state */
	enum osmo_gsup_session_state	session_state;
	/*! Unique session identifier and origination flag.
         * Encoded only when \ref session_state != 0x00 */
	uint32_t			session_id;

	/*! ASN.1 encoded MAP payload for Supplementary Services */
	uint8_t				*ss_info;
	size_t				ss_info_len;
};

int osmo_gsup_decode(const uint8_t *data, size_t data_len,
		     struct osmo_gsup_message *gsup_msg);
int osmo_gsup_encode(struct msgb *msg, const struct osmo_gsup_message *gsup_msg);
int osmo_gsup_get_err_msg_type(enum osmo_gsup_message_type type_in);

/*! @} */
