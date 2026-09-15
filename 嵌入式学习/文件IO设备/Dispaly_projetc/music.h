/* music.h - 音乐模块接口 */
#ifndef __MUSIC_H__
#define __MUSIC_H__

/* 扫描 ./music_lib 目录下的 wav 文件，返回歌曲数量（0 = 没有） */
int  music_init(void);

/* 曲目总数 */
int  music_count(void);

/* 播放第 idx 首（自动停止当前） */
void music_play(int idx);

/* 停止当前播放 */
void music_stop(void);

/* 是否正在播放 */
int  music_is_playing(void);

/* 当前曲目下标，-1 = 没有 */
int  music_current(void);

#endif
