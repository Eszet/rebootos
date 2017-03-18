// $Id: module.c 52 2006-09-21 13:40:50Z jlieder $

#include <module.h>
#include <string.h>
#include <console.h>

static struct mod_entry mod_list[MAXMOD];
static int mod_count = 0;
static bool mod_verbose_output = FALSE;

bool mod_set_verbosity(bool val)
{
  return(mod_verbose_output = val);
}

bool mod_recurse(struct mod_entry *entry)
{
  if(!entry) return(FALSE);

  char strbuf[60];
  strz mod_name = entry->mod_name;

  bool be_verbose = 
    mod_verbose_output && mod_name &&
    (strlen(mod_name) > 0);
  
  if(be_verbose)
  {
    snprintf(
      strbuf, 60, "Setting up %s...", mod_name
    );
    
    print(strbuf);
  }
  
  // start module...
  
  bool success = entry->mod_startup(
    entry->mod_param
  );
  
  if(be_verbose)
  {
    print(success ? "Done." : "Failed!");
    print("\n");
  }
  
  // start "child" modules
    
  int count = entry->needed_count;

  for(int i = 0; i < count; i++)
  {
    struct mod_entry *cur =
      entry->needed_by[i];
      
    if(!mod_recurse(cur))
      return(FALSE);
  }
  
  return(TRUE);
}

bool mod_init(void)
{
  for(int i = 0; i < mod_count; i++)
  {
    struct mod_entry *cur = &mod_list[i];

    // start recursion here...
    if(cur && cur->depend_count == 0)
      if(!mod_recurse(cur))
        return(FALSE);
  }
  
  // all modules were started successfully
  
  return(TRUE);
}

struct mod_entry *mod_register(strz name, mod_init_handler startup)
{
  if(mod_count < MAXMOD - 1)
  {
    struct mod_entry *p;
    p = &mod_list[mod_count++];

    p->mod_name = name;
    p->mod_param = NULL;
    p->mod_startup = startup;
    
    p->depend_count = 0;
    p->needed_count = 0;
    
    return(p);
  }
  
  return(NULL);
}

struct mod_entry *mod_add_dep(struct mod_entry *entry, strz dep)
{
  if(entry && dep)
  {
    struct mod_entry *cur = NULL;
    
    for(int i = 0; i < mod_count; i++)
    {
      cur = &mod_list[i];
      
      if(strcmp(cur->mod_name, dep) == 0)
        break;   
    }
    
    if(cur)
    {
      if(entry->depend_count < 4)
      {
        int index = entry->depend_count++;
        entry->depends_on[index] = cur;
      }
      else
        return(FALSE);	// too much dependencies
      
      if(cur->needed_count < 4)
      {
        int index = cur->needed_count++;
        cur->needed_by[index] = entry;
      }
      else
        return(FALSE);	// too much dependencies
    }
    else
      return(NULL);	// no such entry (dep)
  }
  
  return(entry);
}
