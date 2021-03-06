/*
 * Copyright (c) 2014-2016 Cesanta Software Limited
 * All rights reserved
 */

/* TODO(mkm): remove any reference to the v7 context when building without v7 */
struct v7;
struct v7 *v7;

#ifndef CS_DISABLE_JS

#include <math.h>
#include <stdlib.h>
#include <ets_sys.h>

#include "v7/v7.h"
#include "fw/platforms/esp8266/user/v7_esp.h"
#include "fw/src/sj_v7_ext.h"
#include "common/platforms/esp8266/rboot/rboot/appcode/rboot-api.h"
#include "common/cs_dbg.h"

/*
 * dsleep(time_us[, option])
 *
 * time_us - time in microseconds.
 * option - it specified, system_deep_sleep_set_option is called prior to doing
 *to sleep.
 * The most useful seems to be 4 (keep RF off on wake up, reduces power
 *consumption).
 *
 */

static enum v7_err dsleep(struct v7 *v7, v7_val_t *res) {
  enum v7_err rcode = V7_OK;
  v7_val_t time_v = v7_arg(v7, 0);
  double time = v7_get_double(v7, time_v);
  v7_val_t flags_v = v7_arg(v7, 1);
  uint8 flags = v7_get_double(v7, flags_v);

  if (!v7_is_number(time_v) || time < 0) {
    *res = v7_mk_boolean(v7, false);
    goto clean;
  }
  if (v7_is_number(flags_v)) {
    if (!system_deep_sleep_set_option(flags)) {
      *res = v7_mk_boolean(v7, false);
      goto clean;
    }
  }

  system_deep_sleep((uint32_t) time);

  *res = v7_mk_boolean(v7, true);
  goto clean;

clean:
  return rcode;
}

/*
 * Crashes the process/CPU. Useful to attach a debugger until we have
 * breakpoints.
 */
static enum v7_err crash(struct v7 *v7, v7_val_t *res) {
  (void) v7;
  (void) res;

  *(int *) 1 = 1;
  return V7_OK;
}

/*
 * Returns 0 if current rom = previous, 1 otherwise. Debug only function.
 * (PS: it is hard to store on/remove from flash something during
 * upgrade test, because upgrader brings it back)
 * After flashing it will be 0/0, fater upgrade 0/1, 1/0 etc
 * TODO(alashkin): add a way to detect failed update etc
 */
static enum v7_err is_rboot_updated(struct v7 *v7, v7_val_t *res) {
  rboot_config cfg = rboot_get_config();
  int current = cfg.current_rom;
  int prev = cfg.previous_rom;
  LOG(LL_DEBUG, ("Current ROM: %d, Prev: %d", current, prev));
  *res = v7_mk_boolean(v7, current != prev);
  return V7_OK;
}

void init_v7(void *stack_base) {
  struct v7_create_opts opts;

#ifdef V7_THAW
  opts.object_arena_size = 85;
  opts.function_arena_size = 16;
  opts.property_arena_size = 170;
#else
  opts.object_arena_size = 164;
  opts.function_arena_size = 26;
  opts.property_arena_size = 400;
#endif
  opts.c_stack_base = stack_base;
  v7 = v7_create_opt(opts);

  v7_set_method(v7, v7_get_global(v7), "dsleep", dsleep);
  v7_set_method(v7, v7_get_global(v7), "crash", crash);
  v7_set_method(v7, v7_get_global(v7), "is_rboot_updated", is_rboot_updated);
}

#ifndef V7_NO_FS
void run_init_script() {
  v7_val_t res;

  if (v7_exec_file(v7, "sys_init.js", &res) != V7_OK) {
    sj_print_exception(v7, res, "Init error");
  }
}
#endif /* V7_NO_FS */
#endif /* CS_DISABLE_JS */
