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
	CURLcode res;//״̬��
	
	FILE *fp=fopen(filename,"w");
	
	if (!fp) {
		cout << "fopen ERROR." << endl;
		return -1;
	}
	curl = curl_easy_init();//��ʼ��
	if (curl != 0) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");//�������ص�ַ
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
	//����
	DES_cblock key;
	//�����Կ  
	DES_random_key(&key);
	printf("key:");
	for (int i = 0; i < sizeof(key); i++)
		printf("%02x", key[i]);
	printf("\n");

	DES_key_schedule schedule;
	//ת����schedule  
	DES_set_key_checked(&key, &schedule);
	const_DES_cblock input;
	DES_cblock output;
	size_t n;
	fp = fopen(filename, "rb");
	FILE *fp_enc = fopen(filename_enc, "ab+");//�´�ʹ��ǰҪ��enc.txtɾ��
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
		fwrite(output, sizeof(char), 8, fp_enc);//����̶�8�ֽ�

	}
	fclose(fp);
	fclose(fp_enc);
	system("pause");
	//ѹ��
	
	fp_enc = fopen("enc.txt", "rb");
	FILE *fp_com = fopen("com.zip","wb");
	fseek(fp_enc, 0, SEEK_END);
	uLong filelen = ftell(fp_enc);
	rewind(fp_enc);
	char *text = (char*)malloc(sizeof(char)*filelen+1);
	
	fread(text, sizeof(char), filelen, fp_enc);
	char* buf = NULL;
	/* ���㻺������С����Ϊ������ڴ� */
	uLong buflen = compressBound(filelen);	/* ѹ����ĳ����ǲ��ᳬ��buflen�� */
	if ((buf = (char*)malloc(sizeof(char) * buflen)) == NULL)
	{
		printf("no enough memory!\n");
		return -1;
	}
	
	/* ѹ�� */
	if (compress((Bytef *)buf, &buflen, (Bytef*)text, filelen) != Z_OK)
	{
		printf("compress failed!\n");
		return -1;
	}
	fwrite(buf, sizeof(char), buflen, fp_com);
	/* ��ѹ�� */
	/*if (uncompress((Bytef *)text, &filelen, (Bytef *)buf, buflen) != Z_OK)
	{
		printf("uncompress failed!\n");
		return -1;
	}*/
	/* ��ӡ��������ͷ��ڴ� */
	/*text[filelen] = '\0';
	printf("%s\n\n", text);*/
	
	free(buf);
	fclose(fp_enc);
	fclose(fp_com);
	system("pause");
}
