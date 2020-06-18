#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#pragma warning (disable:4996);//��������硰scanf��fopen��Ҫ�滻Ϊscanf_s��fopen_s����������

// ͳ���ַ�Ƶ�ȵ���ʱ���

typedef struct {
	unsigned char uch;			// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long weight;		// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
} TmpNode;

// �����������
typedef struct {
	unsigned char uch;				// ��8bitsΪ��Ԫ���޷����ַ�
	unsigned long weight;			// ÿ�ࣨ�Զ����Ʊ������֣��ַ�����Ƶ��
	char* code;						// �ַ���Ӧ�Ĺ��������루��̬����洢�ռ䣩
	int parent, lchild, rchild;		// ����˫�׺����Һ���
} HufNode, * HufTree;

// ѡ����С�ʹ�С��������㣬����������������
void select(HufNode* huf_tree, unsigned int n, int* s1, int* s2)
{
	// ����С
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
	huf_tree[*s1].parent = 1;   // ��Ǵ˽���ѱ�ѡ��

// �Ҵ�С
	min = ULONG_MAX;
	for (i = 0; i < n; ++i)
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
}

// ������������
void CreateTree(HufNode* huf_tree, unsigned int char_kinds, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	for (i = char_kinds; i < node_num; ++i)
	{
		select(huf_tree, i, &s1, &s2);		// ѡ����С���������
		huf_tree[s1].parent = huf_tree[s2].parent = i;
		huf_tree[i].lchild = s1;
		huf_tree[i].rchild = s2;
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight;
	}
}

// ���ɹ���������
void HufCode(HufNode* huf_tree, unsigned char_kinds)
{
	unsigned int i;
	int cur, next, index;
	char* code_tmp = (char*)malloc(256 * sizeof(char));		// �ݴ���룬���256��Ҷ�ӣ����볤�Ȳ�����255
	code_tmp[256 - 1] = '\0';

	for (i = 0; i < char_kinds; ++i)
	{
		index = 256 - 1;	// ������ʱ�ռ�������ʼ��

		// ��Ҷ�����������������
		for (cur = i, next = huf_tree[i].parent; next != 0;
			cur = next, next = huf_tree[next].parent)
			if (huf_tree[next].lchild == cur)
				code_tmp[--index] = '0';	// ��0��
			else
				code_tmp[--index] = '1';	// �ҡ�1��
		huf_tree[i].code = (char*)malloc((256 - index) * sizeof(char));			// Ϊ��i���ַ����붯̬����洢�ռ� 
		strcpy(huf_tree[i].code, &code_tmp[index]);     // ���򱣴���뵽�������Ӧ����
	}
	free(code_tmp);		// �ͷű�����ʱ�ռ�
}

// ѹ������
int compress(char* ifname, char* ofname)
{
	unsigned int i, j;
	unsigned int char_kinds;		// �ַ�����
	unsigned char char_temp;		// �ݴ�8bits�ַ�
	unsigned long file_len = 0;
	FILE* infile, * outfile;
	TmpNode node_temp;
	unsigned int node_num;
	HufTree huf_tree;
	char code_buf[256] = "\0";		// ������뻺����
	unsigned int code_len;
	/*
	** ��̬����256����㣬�ݴ��ַ�Ƶ�ȣ�
	** ͳ�Ʋ������������������ͷ�
	*/
	TmpNode* tmp_nodes = (TmpNode*)malloc(256 * sizeof(TmpNode));

	// ��ʼ���ݴ���
	for (i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i;		// �����256���±���256���ַ���Ӧ
	}

	// �����ļ�����ȡ�ַ�Ƶ��
	infile = fopen(ifname, "rb");
	// �ж������ļ��Ƿ����
	if (infile == NULL)
		return -1;
	fread((char*)&char_temp, sizeof(unsigned char), 1, infile);		// ����һ���ַ�
	while (!feof(infile))
	{
		++tmp_nodes[char_temp].weight;		// ͳ���±��Ӧ�ַ���Ȩ�أ����������������ʿ���ͳ���ַ�Ƶ��
		++file_len;
		fread((char*)&char_temp, sizeof(unsigned char), 1, infile);		// ����һ���ַ�
	}
	fclose(infile);

	// ���򣬽�Ƶ��Ϊ��ķ�����޳�
	for (i = 0; i < 256 - 1; ++i)
		for (j = i + 1; j < 256; ++j)
			if (tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}

	// ͳ��ʵ�ʵ��ַ����ࣨ���ִ�����Ϊ0��
	for (i = 0; i < 256; ++i)
		if (tmp_nodes[i].weight == 0)
			break;
	char_kinds = i;

	if (char_kinds == 1)
	{
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		fwrite((char*)&char_kinds, sizeof(unsigned int), 1, outfile);		// д���ַ�����
		fwrite((char*)&tmp_nodes[0].uch, sizeof(unsigned char), 1, outfile);		// д��Ψһ���ַ�
		fwrite((char*)&tmp_nodes[0].weight, sizeof(unsigned long), 1, outfile);		// д���ַ�Ƶ�ȣ�Ҳ�����ļ�����
		free(tmp_nodes);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// �����ַ������������㽨������������������ 
		huf_tree = (HufNode*)malloc(node_num * sizeof(HufNode));		// ��̬������������������     

		// ��ʼ��ǰchar_kinds�����
		for (i = 0; i < char_kinds; ++i)
		{
			// ���ݴ�����ַ���Ƶ�ȿ����������
			huf_tree[i].uch = tmp_nodes[i].uch;
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = 0;
		}
		free(tmp_nodes); // �ͷ��ַ�Ƶ��ͳ�Ƶ��ݴ���

		// ��ʼ����node_num-char_kins�����
		for (; i < node_num; ++i)
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num);		// ������������

		HufCode(huf_tree, char_kinds);		// ���ɹ���������

		// д���ַ�����ӦȨ�أ�����ѹʱ�ؽ���������
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		fwrite((char*)&char_kinds, sizeof(unsigned int), 1, outfile);		// д���ַ�����
		for (i = 0; i < char_kinds; ++i)
		{
			fwrite((char*)&huf_tree[i].uch, sizeof(unsigned char), 1, outfile);			// д���ַ��������򣬶�����˳�򲻱䣩
			fwrite((char*)&huf_tree[i].weight, sizeof(unsigned long), 1, outfile);		// д���ַ���ӦȨ��
		}

		// �������ַ���Ȩ����Ϣ����д���ļ����Ⱥ��ַ�����
		fwrite((char*)&file_len, sizeof(unsigned long), 1, outfile);		// д���ļ�����
		infile = fopen(ifname, "rb");		// �Զ�������ʽ�򿪴�ѹ�����ļ�
		fread((char*)&char_temp, sizeof(unsigned char), 1, infile);     // ÿ�ζ�ȡ8bits
		while (!feof(infile))
		{
			// ƥ���ַ���Ӧ����
			for (i = 0; i < char_kinds; ++i)
				if (char_temp == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);

			// ��8λ��һ���ֽڳ��ȣ�Ϊ����Ԫ
			while (strlen(code_buf) >= 8)
			{
				char_temp = '\0';		// ����ַ��ݴ�ռ䣬��Ϊ�ݴ��ַ���Ӧ����
				for (i = 0; i < 8; ++i)
				{
					char_temp <<= 1;		// ����һλ��Ϊ��һ��bit�ڳ�λ��
					if (code_buf[i] == '1')
						char_temp |= 1;		// ������Ϊ"1"��ͨ���������������ӵ��ֽڵ����λ
				}
				fwrite((char*)&char_temp, sizeof(unsigned char), 1, outfile);		// ���ֽڶ�Ӧ��������ļ�
				strcpy(code_buf, code_buf + 8);		// ���뻺��ȥ���Ѵ����ǰ��λ
			}
			fread((char*)&char_temp, sizeof(unsigned char), 1, infile);     // ÿ�ζ�ȡ8bits
		}
		// ���������8bits����
		code_len = strlen(code_buf);
		if (code_len > 0)
		{
			char_temp = '\0';
			for (i = 0; i < code_len; ++i)
			{
				char_temp <<= 1;
				if (code_buf[i] == '1')
					char_temp |= 1;
			}
			char_temp <<= 8 - code_len;       // �������ֶδ�β���Ƶ��ֽڵĸ�λ
			fwrite((char*)&char_temp, sizeof(unsigned char), 1, outfile);       // �������һ���ֽ�
		}

		// �ر��ļ�
		fclose(infile);
		fclose(outfile);

		// �ͷ��ڴ�
		for (i = 0; i < char_kinds; ++i)
			free(huf_tree[i].code);
		free(huf_tree);
	}
}//compress



// ��ѹ����
int extract(char* ifname, char* ofname)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0;		// �����ļ�д�볤��
	FILE* infile, * outfile;
	unsigned int char_kinds;		// �洢�ַ�����
	unsigned int node_num;
	HufTree huf_tree;
	unsigned char code_temp;		// �ݴ�8bits����
	unsigned int root;		// ������ڵ���������ƥ�����ʹ��

	infile = fopen(ifname, "rb");		// �Զ����Ʒ�ʽ��ѹ���ļ�
	// �ж������ļ��Ƿ����
	if (infile == NULL)
		return -1;

	// ��ȡѹ���ļ�ǰ�˵��ַ�����Ӧ���룬�����ؽ���������
	fread((char*)&char_kinds, sizeof(unsigned int), 1, infile);     // ��ȡ�ַ�������
	if (char_kinds == 1)
	{
		fread((char*)&code_temp, sizeof(unsigned char), 1, infile);     // ��ȡΨһ���ַ�
		fread((char*)&file_len, sizeof(unsigned long), 1, infile);     // ��ȡ�ļ�����
		outfile = fopen(ofname, "wb");					// ��ѹ�������ɵ��ļ�
		while (file_len--)
			fwrite((char*)&code_temp, sizeof(unsigned char), 1, outfile);
		fclose(infile);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_kinds - 1;		// �����ַ������������㽨������������������ 
		huf_tree = (HufNode*)malloc(node_num * sizeof(HufNode));		// ��̬��������������ռ�
		// ��ȡ�ַ�����ӦȨ�أ�������������ڵ�
		for (i = 0; i < char_kinds; ++i)
		{
			fread((char*)&huf_tree[i].uch, sizeof(unsigned char), 1, infile);		// �����ַ�
			fread((char*)&huf_tree[i].weight, sizeof(unsigned long), 1, infile);	// �����ַ���ӦȨ��
			huf_tree[i].parent = 0;
		}
		// ��ʼ����node_num-char_kins������parent
		for (; i < node_num; ++i)
			huf_tree[i].parent = 0;

		CreateTree(huf_tree, char_kinds, node_num);		// �ؽ�������������ѹ��ʱ��һ�£�

		// �����ַ���Ȩ����Ϣ�������Ŷ�ȡ�ļ����Ⱥͱ��룬���н���
		fread((char*)&file_len, sizeof(unsigned long), 1, infile);	// �����ļ�����
		outfile = fopen(ofname, "wb");		// ��ѹ�������ɵ��ļ�
		root = node_num - 1;
		while (1)
		{
			fread((char*)&code_temp, sizeof(unsigned char), 1, infile);		// ��ȡһ���ַ����ȵı���

			// �����ȡ��һ���ַ����ȵı��루ͨ��Ϊ8λ��
			for (i = 0; i < 8; ++i)
			{
				// �ɸ�����ֱ��Ҷ�ڵ�����ƥ������Ӧ�ַ�
				if (code_temp & 128)
					root = huf_tree[root].rchild;
				else
					root = huf_tree[root].lchild;

				if (root < char_kinds)
				{
					fwrite((char*)&huf_tree[root].uch, sizeof(unsigned char), 1, outfile);
					++writen_len;
					if (writen_len == file_len) break;		// �����ļ����ȣ������ڲ�ѭ��
					root = node_num - 1;        // ��λΪ��������ƥ����һ���ַ�
				}
				code_temp <<= 1;		// �����뻺�����һλ�Ƶ����λ����ƥ��
			}
			if (writen_len == file_len) break;		// �����ļ����ȣ��������ѭ��
		}

		// �ر��ļ�
		fclose(infile);
		fclose(outfile);

		// �ͷ��ڴ�
		free(huf_tree);
	}
}//extract()

int main()
{
	while (1)
	{
		int flag = 0;		// ÿ�ν���ѭ����Ҫ��ʼ��flagΪ0
		char opt;
		char ifname[256] = { 0 }, ofname[256] = { 0 };		// ������������ļ���
		// ������ѡ��������͵����ִ��ţ�c��ѹ����e����ѹ��q���˳�
		printf("Please input the number of operations:\n c: compress\n e: extract\n q: quit\n");
		//fflush(stdout);
		scanf("%c",&opt);
		//fflush(stdout);
		if(opt == 'q')
			break;
		else
		{
			printf("Please input the infile name:");
			//fflush(stdout);       //�����������������ѻ������������
			//fflush(stdin);		//�����׼���������Ѷ����δ����������ݶ���,��ֹ����gets������ȡ�ļ���
			scanf("%*[^\n]");       //�������뻺�����е�һ��\n֮ǰ�������ַ�
			scanf("%*c");           //�������뻺�����е�һ���ַ���Ҳ�����ϴ������µ�\n
			//getchar();            //�ӻ���������һ���ַ����൱�����������
			gets(ifname);
			//fflush(stdout);
			printf("Please input the outfile name:");
			//fflush(stdin);
			gets(ofname);
			//fflush(stdout);
		}
		switch (opt)
		{
		case 'c': puts("Compressing����");
			flag = compress(ifname, ofname);	// ѹ��������ֵ�����ж��ļ����Ƿ񲻴���
			break;
		case 'e': puts("Extracting����");
			flag = extract(ifname, ofname);		// ��ѹ������ֵ�����ж��ļ����Ƿ񲻴���
			break;
		}
		if (flag == -1)
			puts("Sorry, infile \"%s\" doesn't exist!", ifname);		// �����־Ϊ��-1���������ļ�������
		else
			puts("Operation is done!\n");		// �������
	}
	return 0;
}