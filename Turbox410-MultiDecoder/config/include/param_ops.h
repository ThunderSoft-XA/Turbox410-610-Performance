#ifndef __PARAM_H__
#define __PARAM_H__
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct __camera_conf
{
	char camera_dic[16];
	int cameraID;
	char cameraName[16];
	char decode[5];//h264 h265
	char enable[4]; //on off
	int height;
	int width;
	/*
	int w_x;
	int w_y;
	int w_w;
	int w_h;
	*/
	char path[1024];
	int rtsp;// 1 rtsp流 0 file
	int ai;// 1 rtsp流 0 file
}ST_CameraConf;

int param_init(void);
void param_deinit(void);
int param_set_int(const char *section, const char *key, int val);
const char *param_get_string(const char *section, const char *key, const char *notfound);
int param_get_int(const char *section, const char *key, int notfound);

int param_set_string(const char *section, const char *key, char *val);
int camera_param_load(char *fileName, ST_CameraConf* pstCameraConf);
#ifdef __cplusplus
}
#endif
#endif
