#include "pch.h"

#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include "output.h"

Output* NewOutput(int bufferSize, goCallback callback) {
	/*
	struct Output {
		char* data;
		int len;
		goCallback callback;
	};*/
	// 1. 动态分配 Output 结构体（在堆上，函数结束后不释放）
	struct Output* output = (struct Output*)malloc(sizeof(struct Output));
	if (output == NULL) {
		printf("内存分配失败：无法创建 Output 结构体\n");
		return NULL;
	}

	// 2. 初始化成员变量
	output->len = bufferSize;

	// 分配数据缓冲区
	output->data = (char*)malloc(bufferSize);
	if (output->data == NULL) {
		printf("内存分配失败：无法创建数据缓冲区\n");
		free(output);  // 释放已分配的结构体，避免内存泄漏
		return NULL;
	}

	memset(output->data, 0, output->len);  // 初始化缓冲区为0
	output->callback = callback;

	return output;  // 返回堆上的结构体地址（有效）
}

/// <summary>
/// 可变长字符串缓冲区
/// </summary>
/// <param name="output"></param>
/// <param name="format"></param>
/// <param name=""></param>
void append(Output* output, const char* format, ...)
{
	// current output length
	int n = strlen((*output).data);

	// length of what we append
	va_list args;
	va_start(args, format);
	int l = vsnprintf(0, 0, format, args);
	va_end(args);

	// grow buffer if needed
	// 如果剩余空间不够（含 \0），就把缓冲区按 2 倍连续 realloc，直到够用。
	while ((n + l + 1) > (*output).len) {
		(*output).len = (*output).len * 2;
		(*output).data = (char*)realloc((*output).data, (*output).len);
	}

	// append to output
	va_start(args, format);
	vsnprintf((*output).data + strlen((*output).data), l + 1, format, args);
	va_end(args);
}

int success(Output* output)
{
	(*output).callback((*output).data, strlen((*output).data));
	free((*output).data);
	return 0;
}

int failure(Output* output)
{
	(*output).callback((*output).data, strlen((*output).data));
	free((*output).data);
	return 1;
}
