#pragma warning (disable:4996)
#include <curl/curl.h>   
#include <openssl/des.h> 
#include<zlib.h>
#include<iostream>
#include<stdio.h>
#include<string.h>
const char* filename = "1.txt";
const char* filename_enc = "enc.txt";
const char* filename_com = "com.zip";
using namespace std;
int main(int argc, char* argv[]) {
	CURL *curl = 0;
	CURLcode res;//状态码
	
	FILE *fp=fopen(filename,"w");
	
	if (!fp) {
		cout << "fopen ERROR." << endl;
		return -1;
	}
	curl = curl_easy_init();//初始化
	if (curl != 0) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");//设置下载地址
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	fclose(fp);
	printf("get Done\n");
	system("pause");
	//加密
	DES_cblock key;
	//随机密钥  
	DES_random_key(&key);
	printf("key:");
	for (int i = 0; i < sizeof(key); i++)
		printf("%02x", key[i]);
	printf("\n");

	DES_key_schedule schedule;
	//转换成schedule  
	DES_set_key_checked(&key, &schedule);
	const_DES_cblock input;
	DES_cblock output;
	size_t n;
	fp = fopen(filename, "rb");
	FILE *fp_enc = fopen(filename_enc, "ab+");//下次使用前要把enc.txt删掉
	if (!fp||!fp_enc) {
		cout << "fopen ERROR." << endl;
		return -1;
	}
	while ((n=fread(input, sizeof(char), 8, fp))>0)
	{
		DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
		for (int i = 0; i < sizeof(input); i++)
			printf("%02x", output[i]);
		printf("\n");
		fwrite(output, sizeof(char), 8, fp_enc);//输出固定8字节

	}
	fclose(fp);
	fclose(fp_enc);
	system("pause");
	//压缩
	
	fp_enc = fopen("enc.txt", "rb");
	FILE *fp_com = fopen("com.zip","wb");
	fseek(fp_enc, 0, SEEK_END);
	uLong filelen = ftell(fp_enc);
	rewind(fp_enc);
	char *text = (char*)malloc(sizeof(char)*filelen+1);
	
	fread(text, sizeof(char), filelen, fp_enc);
	char* buf = NULL;
	/* 计算缓冲区大小，并为其分配内存 */
	uLong buflen = compressBound(filelen);	/* 压缩后的长度是不会超过buflen的 */
	if ((buf = (char*)malloc(sizeof(char) * buflen)) == NULL)
	{
		printf("no enough memory!\n");
		return -1;
	}
	
	/* 压缩 */
	if (compress((Bytef *)buf, &buflen, (Bytef*)text, filelen) != Z_OK)
	{
		printf("compress failed!\n");
		return -1;
	}
	fwrite(buf, sizeof(char), buflen, fp_com);
	/* 解压缩 */
	/*if (uncompress((Bytef *)text, &filelen, (Bytef *)buf, buflen) != Z_OK)
	{
		printf("uncompress failed!\n");
		return -1;
	}*/
	/* 打印结果，并释放内存 */
	/*text[filelen] = '\0';
	printf("%s\n\n", text);*/
	
	free(buf);
	fclose(fp_enc);
	fclose(fp_com);
	system("pause");
}
