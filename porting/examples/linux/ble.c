/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "host/ble_hs.h"
#include "host/util/util.h"

/* scan_event() calls scan(), so forward declaration is required */
static void scan(void);

static void
ble_app_set_addr(void)
{
    ble_addr_t addr;
    int rc;

    /* generate new non-resolvable private address */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert(rc == 0);

    /* set generated address */
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);
}

static void
print_uuid(const ble_uuid_t *uuid)
{
    char buf[BLE_UUID_STR_LEN];

    printf( "%s", ble_uuid_to_str(uuid, buf));
}

/* Utility function to log an array of bytes. */
static void
print_bytes(const uint8_t *bytes, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        printf( "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

static char *
addr_str(const void *addr)
{
    static char buf[6 * 2 + 5 + 1];
    const uint8_t *u8p;

    u8p = addr;
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
            u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);

    return buf;
}

static void
print_adv_fields(const struct ble_hs_adv_fields *fields)
{
    char s[BLE_HS_ADV_MAX_SZ];
    const uint8_t *u8p;
    int i;

    if (fields->flags != 0) {
        printf( "    flags=0x%02x\n", fields->flags);
    }

    if (fields->uuids16 != NULL) {
        printf( "    uuids16(%scomplete)=",
                    fields->uuids16_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids16; i++) {
            print_uuid(&fields->uuids16[i].u);
            printf( " ");
        }
        printf( "\n");
    }

    if (fields->uuids32 != NULL) {
        printf( "    uuids32(%scomplete)=",
                    fields->uuids32_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids32; i++) {
            print_uuid(&fields->uuids32[i].u);
            printf( " ");
        }
        printf( "\n");
    }

    if (fields->uuids128 != NULL) {
        printf( "    uuids128(%scomplete)=",
                    fields->uuids128_is_complete ? "" : "in");
        for (i = 0; i < fields->num_uuids128; i++) {
            print_uuid(&fields->uuids128[i].u);
            printf( " ");
        }
        printf( "\n");
    }

    if (fields->name != NULL) {
        assert(fields->name_len < sizeof s - 1);
        memcpy(s, fields->name, fields->name_len);
        s[fields->name_len] = '\0';
        printf( "    name(%scomplete)=%s\n",
                    fields->name_is_complete ? "" : "in", s);
    }

    if (fields->tx_pwr_lvl_is_present) {
        printf( "    tx_pwr_lvl=%d\n", fields->tx_pwr_lvl);
    }

    if (fields->slave_itvl_range != NULL) {
        printf( "    slave_itvl_range=");
        print_bytes(fields->slave_itvl_range, BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN);
        printf( "\n");
    }

    if (fields->svc_data_uuid16 != NULL) {
        printf( "    svc_data_uuid16=");
        print_bytes(fields->svc_data_uuid16, fields->svc_data_uuid16_len);
        printf( "\n");
    }

    if (fields->public_tgt_addr != NULL) {
        printf( "    public_tgt_addr=");
        u8p = fields->public_tgt_addr;
        for (i = 0; i < fields->num_public_tgt_addrs; i++) {
            printf( "public_tgt_addr=%s ", addr_str(u8p));
            u8p += BLE_HS_ADV_PUBLIC_TGT_ADDR_ENTRY_LEN;
        }
        printf( "\n");
    }

    if (fields->appearance_is_present) {
        printf( "    appearance=0x%04x\n", fields->appearance);
    }

    if (fields->adv_itvl_is_present) {
        printf( "    adv_itvl=0x%04x\n", fields->adv_itvl);
    }

    if (fields->svc_data_uuid32 != NULL) {
        printf( "    svc_data_uuid32=");
        print_bytes(fields->svc_data_uuid32, fields->svc_data_uuid32_len);
        printf( "\n");
    }

    if (fields->svc_data_uuid128 != NULL) {
        printf( "    svc_data_uuid128=");
        print_bytes(fields->svc_data_uuid128, fields->svc_data_uuid128_len);
        printf( "\n");
    }

    if (fields->uri != NULL) {
        printf( "    uri=");
        print_bytes(fields->uri, fields->uri_len);
        printf( "\n");
    }

    if (fields->mfg_data != NULL) {
        printf( "    mfg_data=");
        print_bytes(fields->mfg_data, fields->mfg_data_len);
        printf( "\n");
    }
}

static int
scan_event(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields fields;
    int rc;
    switch (event->type) {
    /* advertising report has been received during discovery procedure */
    case BLE_GAP_EVENT_DISC:
        printf( "Advertising report received!\n");
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            return 0;
        }
        print_adv_fields(&fields);
        return 0;
    /* discovery procedure has terminated */
    case BLE_GAP_EVENT_DISC_COMPLETE:
        printf( "Discovery completed, terminaton code: %d\n",
                    event->disc_complete.reason);
        scan();
        return 0;
    default:
        printf( "Discovery event not handled\n");
        return 0;
    }
}

static void
scan(void)
{
    /* set scan parameters */
    struct ble_gap_disc_params scan_params;
    scan_params.itvl = 500;
    scan_params.window = 250;
    scan_params.filter_policy = 0;
    scan_params.limited = 0;
    scan_params.passive = 1;
    scan_params.filter_duplicates = 1;
    /* performs discovery procedure; value of own_addr_type is hard-coded,
       because NRPA is used */
    ble_gap_disc(BLE_OWN_ADDR_RANDOM, 1000, &scan_params, scan_event, NULL);
}

static void
on_sync(void)
{
    /* Generate a non-resolvable private address. */
    ble_app_set_addr();

    /* begin scanning */
    scan();
}

static void
on_reset(int reason)
{
    printf("Resetting state; reason=%d\n", reason);
}

#if 0
int
mynewt_main(int argc, char **argv)
{
    /* Initialize all packages. */
    sysinit();

    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.reset_cb = on_reset;

    /* As the last thing, process events from default event queue. */
    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }

    return 0;
}
#endif
static const char gap_name[] = "nimble";

static uint8_t own_addr_type;

static void start_advertise(void);

static void
put_ad(uint8_t ad_type, uint8_t ad_len, const void *ad, uint8_t *buf,
       uint8_t *len)
{
    buf[(*len)++] = ad_len + 1;
    buf[(*len)++] = ad_type;

    memcpy(&buf[*len], ad, ad_len);

    *len += ad_len;
}

static void
update_ad(void)
{
    uint8_t ad[BLE_HS_ADV_MAX_SZ];
    uint8_t ad_len = 0;
    uint8_t ad_flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    put_ad(BLE_HS_ADV_TYPE_FLAGS, 1, &ad_flags, ad, &ad_len);
    put_ad(BLE_HS_ADV_TYPE_COMP_NAME, sizeof(gap_name), gap_name, ad, &ad_len);

    ble_gap_adv_set_data(ad, ad_len);
}

static int
gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status) {
            start_advertise();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        start_advertise();
        break;
    }

    return 0;
}

static void
start_advertise(void)
{
    struct ble_gap_adv_params advp;
    int rc;

    update_ad();

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &advp, gap_event_cb, NULL);
    assert(rc == 0);
}

static void
app_ble_sync_cb(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    assert(rc == 0);

    start_advertise();
}

void
nimble_host_task(void *param)
{
    //ble_hs_cfg.sync_cb = app_ble_sync_cb;
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.reset_cb = on_reset;

    ble_svc_gap_device_name_set(gap_name);

    nimble_port_run();
}
