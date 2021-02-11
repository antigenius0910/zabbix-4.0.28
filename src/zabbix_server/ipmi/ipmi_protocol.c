/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include "common.h"

#ifdef HAVE_OPENIPMI

#include "log.h"
#include "zbxserialize.h"

#include "ipmi_protocol.h"

zbx_uint32_t	zbx_ipmi_serialize_request(unsigned char **data, zbx_uint64_t objectid, const char *addr,
		unsigned short port, signed char authtype, unsigned char privilege, const char *username,
		const char *password, const char *sensor, int command)
{
	unsigned char	*ptr;
	zbx_uint32_t	data_len, addr_len, username_len, password_len, sensor_len;

	addr_len = strlen(addr) + 1;
	username_len = strlen(username) + 1;
	password_len = strlen(password) + 1;
	sensor_len = strlen(sensor) + 1;

	data_len = sizeof(zbx_uint64_t) + sizeof(short) + sizeof(char) * 2 + addr_len + username_len + password_len +
			sensor_len + sizeof(zbx_uint32_t) * 4 + sizeof(int);

	*data = (unsigned char *)zbx_malloc(NULL, data_len);
	ptr = *data;
	ptr += zbx_serialize_uint64(ptr, objectid);
	ptr += zbx_serialize_str(ptr, addr, addr_len);
	ptr += zbx_serialize_short(ptr, port);
	ptr += zbx_serialize_char(ptr, authtype);
	ptr += zbx_serialize_char(ptr, privilege);
	ptr += zbx_serialize_str(ptr, username, username_len);
	ptr += zbx_serialize_str(ptr, password, password_len);
	ptr += zbx_serialize_str(ptr, sensor, sensor_len);
	(void)zbx_serialize_int(ptr, command);

	return data_len;
}

void	zbx_ipmi_deserialize_request(const unsigned char *data, zbx_uint64_t *objectid, char **addr,
		unsigned short *port, signed char *authtype, unsigned char *privilege, char **username, char **password,
		char **sensor, int *command)
{
	zbx_uint32_t	value_len;

	data += zbx_deserialize_uint64(data, objectid);
	data += zbx_deserialize_str(data, addr, value_len);
	data += zbx_deserialize_short(data, port);
	data += zbx_deserialize_char(data, authtype);
	data += zbx_deserialize_char(data, privilege);
	data += zbx_deserialize_str(data, username, value_len);
	data += zbx_deserialize_str(data, password, value_len);
	data += zbx_deserialize_str(data, sensor, value_len);
	(void)zbx_deserialize_int(data, command);
}

void	zbx_ipmi_deserialize_request_objectid(const unsigned char *data, zbx_uint64_t *objectid)
{
	(void)zbx_deserialize_uint64(data, objectid);
}

zbx_uint32_t	zbx_ipmi_serialize_result(unsigned char **data, const zbx_timespec_t *ts, int errcode,
		const char *value)
{
	unsigned char	*ptr;
	zbx_uint32_t	data_len, value_len;

	value_len = (NULL != value ? strlen(value)  + 1 : 0);

	data_len = value_len + sizeof(zbx_uint32_t) + sizeof(int) * 3;
	*data = (unsigned char *)zbx_malloc(NULL, data_len);

	ptr = *data;
	ptr += zbx_serialize_int(ptr, ts->sec);
	ptr += zbx_serialize_int(ptr, ts->ns);
	ptr += zbx_serialize_int(ptr, errcode);
	(void)zbx_serialize_str(ptr, value, value_len);

	return data_len;
}

void	zbx_ipmi_deserialize_result(const unsigned char *data, zbx_timespec_t *ts, int *errcode, char **value)
{
	int	value_len;

	data += zbx_deserialize_int(data, &ts->sec);
	data += zbx_deserialize_int(data, &ts->ns);
	data += zbx_deserialize_int(data, errcode);
	(void)zbx_deserialize_str(data, value, value_len);
}

#endif