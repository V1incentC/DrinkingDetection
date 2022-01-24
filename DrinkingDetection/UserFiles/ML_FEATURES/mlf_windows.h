/*
 ******************************************************************************
 * @file    mlf_windows.h
 * @author  Vincent Cergolj
 * @brief   LSM6DSL low level driver file
 ******************************************************************************
*/
/*
 * Copyright (c) 2021 Vincent Cergolj
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of machine learning features.
 *
 * Author:          Vincent Cergolj
 */
#ifndef MLF_WINDOWS_H
#define MLF_WINDOWS_H
#ifdef __cplusplus
  extern "C" {
#endif
/**
  * @defgroup    MLF_WINDOWS
  * @brief       This file contatins arrays of windows specific
  *              to calculation of features for drinking detection.
  * @{
  *
  */
extern  const float mlf_window[256];
      
extern const float mlf_std_vals[294];
      
extern const float mlf_mean_vals[294];



/**
  * @}
  */ 
#ifdef __cplusplus
}
#endif

#endif // !MLF_WINDOWS_H