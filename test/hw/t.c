#include "system/state.h"
#include <assert.h>

static void w(const char *p, const char *s){FILE*f=fopen(p,"w");fputs(s,f);fclose(f);}
static void touch(const char*p){FILE*f=fopen(p,"w");if(f)fclose(f);}
static char *rd(const char*p){static char b[65536];FILE*f=fopen(p,"r");size_t n=f?fread(b,1,sizeof b-1,f):0;b[n]=0;if(f)fclose(f);return b;}
#define R "/tmp/sdtest/Roms/recentlist.json"
int main(void){
  char longname[300]; memset(longname,'A',230); strcpy(longname+230,".gba");
  char rom[512]; snprintf(rom,sizeof rom,"/tmp/sdtest/Roms/GBA/%s",longname);   // 255 chars + dir
  char rom2[]="/tmp/sdtest/Roms/GB/Tetris.gb";
  char launch[]="/tmp/sdtest/Emu/GBA/launch.sh";
  touch(rom); touch(rom2); touch(launch);
  printf("rom path length: %zu\n", strlen(rom));
  char line[4096], buf[RECENT_PATH_MAX];

  // 1 long rompath
  snprintf(line,sizeof line,"{\"label\":\"x\",\"rompath\":\"%s\",\"imgpath\":\"\",\"launch\":\"%s\",\"type\":5}\n",rom,launch); w(R,line);
  printf("1 long path: %s\n", history_getRecentPath(buf,sizeof buf) && !strcmp(buf,rom) ? "ok" : "FAIL");
  char rs[STR_MAX]; printf("1b romscreen: %s\n", history_getRomscreenPath(rs) ? "ok" : "FAIL");
  // 2 search-panel form
  snprintf(line,sizeof line,"{\"label\":\"x\",\"rompath\":\"%s:%s\",\"type\":17}\n",launch,rom2); w(R,line);
  printf("2 launch:rompath: %s\n", history_getRecentPath(buf,sizeof buf) && !strcmp(buf,rom2) ? "ok" : "FAIL");
  // 3 malformed / missing keys / app / empty
  w(R,"{\"label\":\"x\"}\n"); printf("3 no type: %s\n", history_getRecentPath(buf,sizeof buf)==NULL?"ok":"FAIL");
  w(R,"{\"type\":5,\"rom\n"); printf("3b broken json: %s\n", history_getRecentPath(buf,sizeof buf)==NULL?"ok":"FAIL");
  w(R,"{\"label\":\"app\",\"rompath\":\"/x\",\"type\":3}\n"); printf("3c app entry: %s\n", history_getRecentPath(buf,sizeof buf)==NULL?"ok":"FAIL");
  w(R,""); printf("3d empty: %s\n", history_getRecentPath(buf,sizeof buf)==NULL?"ok":"FAIL");
  char small[8]; snprintf(line,sizeof line,"{\"rompath\":\"%s\",\"launch\":\"%s\",\"type\":5}\n",rom2,launch); w(R,line);
  printf("3e small out buffer: %s\n", history_getRecentPath(small,sizeof small)==NULL?"ok":"FAIL");

  // 4 resumeGame(1) over junk, app, missing rom, a 1200-byte valid entry, then target
  char big_label[700]; memset(big_label,'L',699); big_label[699]=0;
  char l1[]="garbage line\n";
  char l2[]="{\"label\":\"app\",\"rompath\":\"/tmp/sdtest/App/x\",\"launch\":\"/x\",\"type\":3}\n";
  char l3[512]; snprintf(l3,sizeof l3,"{\"rompath\":\"/tmp/sdtest/Roms/missing.gb\",\"launch\":\"%s\",\"type\":5}\n",launch);
  char l4[2048]; snprintf(l4,sizeof l4,"{\"label\":\"%s\",\"rompath\":\"%s\",\"imgpath\":\"\",\"launch\":\"%s\",\"type\":5}\n",big_label,rom,launch);
  char l5[512]; snprintf(l5,sizeof l5,"{\"label\":\"Tetris\",\"rompath\":\"%s\",\"launch\":\"%s\",\"type\":5}\n",rom2,launch);
  printf("long line length: %zu\n", strlen(l4));
  char all[8192]; snprintf(all,sizeof all,"%s%s%s%s%s",l1,l2,l3,l4,l5); w(R,all);
  resumeGame(1);
  char *cmd=rd(CMD_TO_RUN_PATH);
  printf("4 resume index 1 -> Tetris: %s\n", strstr(cmd,rom2)?"ok":"FAIL");
  char expect[8192]; snprintf(expect,sizeof expect,"%s%s%s%s%s",l5,l1,l2,l3,l4);
  printf("4b list moved to top, intact: %s\n", !strcmp(rd(R),expect)?"ok":"FAIL");
  // 5 resumeGame(0) picks the long-path game
  w(R,all); resumeGame(0); cmd=rd(CMD_TO_RUN_PATH);
  printf("5 resume index 0 -> long path: %s\n", strstr(cmd,rom)?"ok":"FAIL");
  // 6 file_delete_line / file_read_lineN with long lines
  w(R,all); file_delete_line(R,4); snprintf(expect,sizeof expect,"%s%s%s%s",l1,l2,l3,l5);
  printf("6 delete line 4 (long): %s\n", !strcmp(rd(R),expect)?"ok":"FAIL");
  w(R,all); char *n5=file_read_lineN(R,5); printf("6b read line 5: %s\n", n5&&!strcmp(n5,l5)?"ok":"FAIL"); free(n5);
  // 7 file_readLastLine on empty file
  w("/tmp/sdtest/empty",""); char last[256]="";
  file_readLastLine("/tmp/sdtest/empty",last); printf("7 readLastLine empty: ok\n");
  return 0;
}
