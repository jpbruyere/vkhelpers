/*
 * Copyright (c) 2018-2022 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VKH_APP_H
#define VKH_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vkh.h"

//console colors for debug output on stdout with debug utils or debug report
#ifdef __unix__
	#define KNRM  "\x1b[0m"
	#define KRED  "\x1B[41m\x1B[37m"
	#define KGRN  "\x1B[42m\x1B[30m"
	#define KYEL  "\x1B[43m\x1B[30m"
	#define KBLU  "\x1B[44m\x1B[30m"
#else
	#define KNRM  ""
	#define KRED  ""
	#define KGRN  ""
	#define KYEL  ""
	#define KBLU  ""
	#define KMAG  ""
	#define KCYN  ""
	#define KWHT  ""
#endif

typedef struct _vkh_app_t{
	VkApplicationInfo	infos;
	VkInstance			inst;
	VkDebugUtilsMessengerEXT debugMessenger;
}vkh_app_t;
#ifdef __cplusplus
}
#endif
#endif
