#include <iniparser.h>
#include "file_ops.h"
#include "param_ops.h"
#include "errno.h"


static char mfileName[1024]={0};
static dictionary *conf_dic;

static FILE *fp_dic = NULL;

int is_file_exist(char *file_path)
{
	if (!file_path)
		return -1;

	if (access(file_path, 0) != -1)
		return 0;

	return -1;
}


/*bref; sync
 * 		employee_time= 
 * 		visitor_time=
 * 		illegal_time= 
 * 		statistic
 * 		recognized_cnt= */
int param_init(void)
{
	int ret = is_file_exist(mfileName);
	if(ret != 0)
	{
		printf("%s: %s not exsit \n",__func__,mfileName);
	return -1;
	}
	conf_dic = iniparser_load(mfileName);
	if (!conf_dic) {
		printf("failed to load app config file:%s", mfileName);
		return -1;
	}


	return 0;
}

void param_deinit(void)
{
	if (conf_dic)
		iniparser_freedict(conf_dic);
	if(fp_dic)
    	fclose(fp_dic);
}


int param_set_int(const char *section, const char *key, int val)
{
	char buf[32];
	int notfound = -1;
	int ret = 0;

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);
	char int_buf[32];
	snprintf(int_buf, sizeof(int_buf), "%d",val);

	printf("%s: buf %s val %d ",__func__,buf,val);

	ret = iniparser_set(conf_dic, (const char *)buf, (const char *)int_buf);

//写入文件
     fp_dic = fopen(mfileName, "w");
    if( fp_dic == NULL ) {
        printf("stone:fopen error!\n");
		return -1;
    }
    iniparser_dump_ini(conf_dic,fp_dic);
	fclose(fp_dic);

	return ret;
}

int param_set_string(const char *section, const char *key, char *val)
{
	char buf[32];
	int notfound = -1;
	int ret = 0;

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);
	printf("%s: buf %s val %s ",__func__,buf,val);

	ret = iniparser_set(conf_dic, (const char *)buf, (const char *)val);
//写入文件
     fp_dic = fopen(mfileName, "w");
    if( fp_dic == NULL ) {
        printf("stone:fopen error!\n");
		return -1;
    }

    iniparser_dump_ini(conf_dic,fp_dic);
	fclose(fp_dic);

	return ret;
}

const char *param_get_string(const char *section, const char *key, const char *notfound)
{
	char buf[32];
	char *str = NULL;

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	str =  (char *)iniparser_getstring(conf_dic, buf, notfound);
	return (const char *)str;
}

int param_get_int(const char *section, const char *key, int notfound)
{
	char buf[32];
	int ret = 0;

	if (!conf_dic)
		return notfound;

	snprintf(buf, sizeof(buf), "%s:%s", section, key);

	ret = iniparser_getint(conf_dic, buf, notfound);

	return ret;
}

int camera_param_load(char *fileName, ST_CameraConf* pstCameraConf)
{
	sprintf(mfileName,"%s",fileName);
	
	int ret = param_init();
	if(ret)
		return -1;

	sprintf(pstCameraConf->cameraName, "%s",param_get_string(pstCameraConf->camera_dic,"cameraname","camera_zero"));

	sprintf(pstCameraConf->decode, "%s",param_get_string(pstCameraConf->camera_dic,"decode","NULL"));
	sprintf(pstCameraConf->enable, "%s",param_get_string(pstCameraConf->camera_dic,"enable","off"));
	sprintf(pstCameraConf->path, "%s",param_get_string(pstCameraConf->camera_dic,"path","NULL"));

	pstCameraConf->cameraID = param_get_int(pstCameraConf->camera_dic,"cameraid",0);
	pstCameraConf->height = param_get_int(pstCameraConf->camera_dic,"height",1920);
	pstCameraConf->width = param_get_int(pstCameraConf->camera_dic,"width",1080);
	pstCameraConf->rtsp = param_get_int(pstCameraConf->camera_dic,"rtsp",1);
	pstCameraConf->ai = param_get_int(pstCameraConf->camera_dic,"ai",1);

	param_deinit();
	printf("%s\n",__FUNCTION__);
	printf("cameraName: %s\n",pstCameraConf->cameraName);
	printf("path: %s\n",pstCameraConf->path);
	printf("rtsp: %d\n",pstCameraConf->rtsp);
	if(strncmp(pstCameraConf->decode, "NULL",strlen("NULL")) == 0 || strncmp(pstCameraConf->path,"NULL",strlen("NULL")) == 0)
	{
		printf("%s: error\n",__FUNCTION__);
		return -1;
	}
	return 0;
}
