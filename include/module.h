#ifndef __MODULE_H__
#define __MODULE_H__

#include <types.h>

// PurpOS Module System

#define MAXMOD 16

typedef bool (*mod_init_handler)(void *resvd);

struct mod_entry
{
  strz mod_name;
  void *mod_param;
  
  mod_init_handler mod_startup;
  
  struct mod_entry *depends_on[4];
  int depend_count;
  
  struct mod_entry *needed_by[4];
  int needed_count;
};

bool mod_set_verbosity(bool val);
bool mod_init(void);
struct mod_entry *mod_register(strz name, mod_init_handler startup);
struct mod_entry *mod_add_dep(struct mod_entry *entry, strz dep);

#define MODULE(name, ident, dep) \
  do { \
    struct mod_entry *_entry = \
      mod_register(name, ident ## _init); \
    mod_add_dep(_entry, dep); \
  } while(0)

#endif
