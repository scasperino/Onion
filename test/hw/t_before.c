// Same long-path case, against the unpatched code (main): history_getRecentPath(char *)
#include "system/state.h"
static void w(const char *p, const char *s){FILE*f=fopen(p,"w");fputs(s,f);fclose(f);}
static void touch(const char*p){FILE*f=fopen(p,"w");if(f)fclose(f);}
int main(int argc, char **argv){
  int n = argc > 1 ? atoi(argv[1]) : 230;
  char longname[300]; memset(longname,'A',n); strcpy(longname+n,".gba");
  char rom[512]; snprintf(rom,sizeof rom,"/tmp/sdtest/Roms/GBA/%s",longname);
  char launch[]="/tmp/sdtest/Emu/GBA/launch.sh"; touch(rom); touch(launch);
  char line[2048]; snprintf(line,sizeof line,"{\"label\":\"x\",\"rompath\":\"%s\",\"imgpath\":\"\",\"launch\":\"%s\",\"type\":5}\n",rom,launch);
  w("/tmp/sdtest/Roms/recentlist.json",line);
  printf("before: rom path length %zu, calling history_getRomscreenPath()\n", strlen(rom));
  char out[256]; printf("result: %d\n", history_getRomscreenPath(out));
  return 0;
}
