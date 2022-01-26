/*
 ******************************************************************************
 * @file    machine_learning_features.h
 * @author  Vincent Cergolj
 * @brief   Functions for calculation machine learning features on IMU signals
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
#ifndef MACHINE_LEARNING_FEATURES_H
#define MACHINE_LEARNING_FEATURES_H
#ifdef __cplusplus
  extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#include "mlf_windows.h"
/**
  * @defgroup    MACHINE_LEARNING_FEATURES
  * @brief       This file contains function for calculating features on IMU signals in time and 
  *              frequency domain. Features are used for a machine learning model that predicts
  *              drinking based on calculated features from this file.
  * @{
  *
  */

#define  MLF_BUFFER_SIZE     256 //length of the array
#define  MLF_FS              52 //sampling frequency of the signal
#define  MLF_TIMESTEP        1/MLF_FS
#define  MLF_WIN_LEN         256 //window length
#define  MLF_OVERLAP         128 //overlap length
#define  MLF_FFT_SIZE        256
#define  MLF_WELCH_OUT_LEN   MLF_FFT_SIZE / 2 + 1
#define  MLF_BIN_SIZE        10
#define  MLF_NUM_OF_FEATURES 294

typedef struct
{
    float value;
    uint32_t index;
}argsort_st;

/**
  * @defgroup    TIME_DOMAIN_FEATURES
  * @brief       This section provides functions used for calculating time domain features.
  * @{
  *
  */ 


/**
  * @defgroup    EATING_DETECTION_FEATURES
  * @brief       This section provides functions used for calculating
  *              time domain features for eating detection.
  * @{
  *
  */ 

/**
  * \brief           Function for computing the arithmetic mean value of a dataset
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval mean     Mean value of dataset
  */  
float mlf_mean_value(float* data, size_t length);

/**
  * \brief           Function for computing the standard deviation of a dataset
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval std      Standard deviation value of dataset
  */ 
float mlf_standard_deviation(float* data, size_t length);

/**
  * \brief           Function for computing the coefficient of variation of a dataset
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval cov      Coefficient of variation value of dataset
  */
float mlf_coefficient_of_variation(float* data, size_t length);

/**
  * \brief           Function for computing the absolute mean value of a dataset.
  *                  It sums up absolute values of a dataset and then calculates the mean value.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval cov      Absolute mean value of dataset
  */
float mlf_abs_mean_value(float* data, size_t length);


/**
  * \brief           Function for computing the number of zero crossings of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval nzc      Number of zero crossings of a dataset
  */
uint16_t mlf_num_of_zero_crossings(float* data, size_t length);


/**
  * \brief           Function for computing the amplitude of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval amp      Amplitude of a dataset
  */
float mlf_amplitude(float *data, size_t length);


/**
  * \brief           Function for computing the velocity of a dataset.
  * 
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Lenght of data
  * \param[in]       timestep:  Inverse of sampling frequency (dT or 1/Fs)
  * 
  * \retval velocity            Velocitiy of a dataset
  */
float mlf_velocity(float* data, size_t length, float timestep);


/**
  * \brief           Function for computing the sum per component of a dataset.
  * 
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Lenght of data
  * \param[in]       timestep:  Inverse of sampling frequency (dT or 1/Fs)
  * 
  * \retval spc                 Sum per component
  */
float mlf_sum_per_component(float* data, size_t length, float timestep);


/**
  * \brief           Function for computing the number of times the signal crosses
  *                  it's mean value.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval mcr      Number of mean crossings
  */
uint16_t mlf_mean_crossing_rate(float* data, size_t length);


/**
  * \brief           Function for computing the entropy of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval entropy          Entropy of dataset
  */
float mlf_entropy(float* data, size_t length);


/**
  * \brief           Function for computing the inter quartile range of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval iqr      IQR of dataset
  */
float mlf_iqr(float* data, size_t length);


/**
  * \brief           Function for computing the kurtosis of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval kurtosis         Kurtosis of dataset
  */
float mlf_kurtosis(float* data, size_t length);


/**
  * \brief           Function for computing the skewness of a dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval skewness         Skewness of dataset
  */
float mlf_skewness(float* data, size_t length);
/**
  * @}
  */ 


/**
  * @defgroup    DRINKING_DETECTION_FEATURES
  * @brief       This section provides functions used for calculating
  *              time domain features for drinking detection.
  * @{
  *
  */ 


/**
  * \brief           Function for computing the sum over the absolute
  *                  value of consecutive changes in the dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval asc              Absolute sum of changes
  */
float mlf_absolute_sum_of_changes(float* data, size_t length);


/**
  * \brief           Function for computing the number of values in the dataset
  *                  that are higher than the mean of the dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval count              Count above mean
  */
uint16_t mlf_count_above_mean(float* data, size_t length);


/**
  * \brief           Function for computing the number of values in the dataset
  *                  that are lower than the mean of the dataset.
  * 
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval count              Count below mean
  */
uint16_t mlf_count_below_mean(float* data, size_t length);


/**
  * \brief           Function for computing the highest value in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval max              Maximum value of dataset
  */
float mlf_maximum(float* data, size_t length);


/**
  * \brief           Function for computing the lowest value in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval max              Lowest value of dataset
  */
float mlf_minimum(float* data, size_t length);


/**
  * \brief           Function for computing the median the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval median           Median value of dataset
  */
float mlf_median(float* data, size_t length);


/**
  * \brief           Function for computing the mean over the absolute differences
  *                  between subsequent time series values in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval mac              Absolute mean change
  */
float mlf_mean_abs_change(float* data, size_t length);


/**
  * \brief           Function for computing the mean over the differences
  *                  between subsequent time series values in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval mc               Mean change
  */
float mlf_mean_change(float* data, size_t length);


/**
  * \brief           Function for computing the length of the longest consecutive
  *                  subsequence in the dataset that is bigger than
  *                  the mean of in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval val              Longest strike above mean
  */
uint16_t mlf_longest_strike_above_mean(float *data, size_t length);


/**
  * \brief           Function for computing the length of the longest consecutive
  *                  subsequence in the dataset that is lower than
  *                  the mean of in the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \retval val              Longest strike below mean
  */
uint16_t mlf_longest_strike_below_mean(float *data, size_t length);

/**
  * @}
  */ 



/**
  * @}
  */ 


/**
  * @defgroup    FREQUECY_DOMAIN_FEATURES
  * @brief       This section provides functions used for calculating time domain features.
  * @{
  *
  */ 
      
      
/**
  * \brief           The returned float array output_array contains the frequency bin
  *                  centers in cycles per unit of the sample spacing (with zero at the start).
  *                  For instance, if the sample spacing is in seconds,
  *                  then the frequency unit is cycles/second.
  *  
  * \param[in]       sampling_frequency:        Dataset we want to execute calculations on
  * \param[in]       length_of_fft:             Size of our fft calculation
  * \param[in]       length_of_output_array:    Number of samples we want to generate
  * \param[out]      output_array:              Pointer to output array
  *
  */      
void mlf_sample_frequencies(uint16_t sampling_frequency,
                            uint16_t length_of_fft,
                            uint16_t length_of_output_array,
                            float* output_array);
/**
  * \brief           This function computes the one-dimensional n-point
  *                  discrete Fourier Transform (DFT) with the efficient
  *                  Fast Fourier Transform (FFT) algorithm [CT].
  *  
  * \param[in]       data_in:           Dataset we want to execute calculations on
  * \param[in]       data_in_length:    Length of input array
  * \param[in]       data_out:          Pointer to output array
  * \param[in]       data_out_length:   Length of output array
  *
  */        
uint8_t mlf_fft_numpy(float* data_in,    size_t data_in_length,
                      float* data_out, uint32_t data_out_length);


/**
  * \brief           This function computes the entropy of
  *                  frequency spectrum of a signal.
  *                  Input array should be complex nubers of format: data[0] = Re0, data[1] = Im0,
  *                                                                  data[2] = Re1, data[3] = Im1
  *  
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Length of input array
  *
  * \retval val      Entropy of frequency spectrum        
  */        
float mlf_spectrum_entropy(float* data, size_t length);
      
 
/**
  * \brief           This function computes the energy of
  *                  frequency spectrum of a signal.
  *                  Input array should be complex nubers of format: data[0] = Re0, data[1] = Im0,
  *                                                                  data[2] = Re1, data[3] = Im1
  *  
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Length of input array
  *
  * \retval val      Energy of frequency spectrum        
  */        
float mlf_spectrum_energy(float* data, size_t length);
      

/**
  * \brief           Function for subtracting the mean of the dataset.
  *                  It removes a linear trend from the dataset.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * 
  */      
void mlf_detrend(float* data, size_t length);      
      

/**
  * \brief           Function for windowing the selected dataset with
  *                  windows provided in mlf_windows.h.
  *                  Windows should be added by the user with the appropriate length.
  *  
  * \param[in]       data:   Dataset we want to execute calculations on
  * \param[in]       length: Lenght of data
  * \param[in]       window: Pointer to window array
  * 
  */      
void mlf_window_data(float* data, size_t length, const float* window);    

      
/**
  * \brief           Function for estimating power spectral density using Welch’s method.
  *                  
  *
  * \param[in]       data:          Dataset we want to execute calculations on
  * \param[in]       length:        Lenght of data
  * \param[in]       window_length: Size of windows used for Welch's method
  * \param[in]       overlap:       Overlap between windows
  * \param[in]       fft_length:    Lenght of fft performed on windows
  * \param[in]       window:        Pointer to window used for signal windowing
  *                                 (Hanning is included in mlf_windows.h).
  * \param[in]       data_out:      Pointer to output array
  * \param[in]       out_length:    Length of output array.
  *                                 This can be calculated with MLF_WELCH_OUT_LEN macro.
  * 
  */       
void mlf_welch_method(float* data, size_t length,
                      uint16_t window_length,
                      uint16_t overlap,
                      uint16_t fft_length,
                      const float* window, 
                      float* data_out, size_t out_length);
      
/**
  * \brief           Function for calculating the squared sum.
  *                  sum += a*a
  *  
  * \param[in]       data:          Dataset we want to execute calculations on
  * \param[in]       start_index:   From where to start summing the array
  * \param[in]       stop_index:    To where to stop summing the array
  * 
  */          
float mlf_squared_sum(float*   data,
                      uint16_t start_index,   uint16_t stop_index);
      

/**
  * \brief           Function for filling a bin array. Usually used to make bins from
  *                  fft results.
  *  
  * \param[in]       data:          Dataset we want to execute calculations on
  * \param[in]       length:        Lenght of data
  * \param[in]       bin_array:     Pointer to bin array
  * \param[in]       bin_length:    Length of the bin array
  * 
  */      
void mlf_fill_bin_array(float* data,      size_t length,
                        float* bin_array, size_t bin_length);
      
/**
  * \brief           Function for sorting an array and keeping track of sorted indices.
  *  
  * \param[in]       data:          Dataset we want to execute calculations on
  * \param[in]       length:        Lenght of data
  * \param[in]       sorted_values: Pointer to a structure that will hold the
  *                                 sorted values and respective indices

  * 
  */       
void mlf_argsort(float*      data,
                 size_t      length,
                 argsort_st* sorted_values);      

/**
  * \brief           Function for filling the feature array with calculated features in time domain.
  *                  This function is application specific.
  *  
  * \param[in]       features:  Features array
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Lenght of data
  *                                 
  */        
void mlf_fill_raw_eating_features(float* features,
                                  float* data, size_t length);
      

/**
  * \brief           Function for filling the feature array with calculated features in time domain.
  *                  This function is application specific.
  *  
  * \param[in]       features:  Features array
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Lenght of data
  *                                 
  */ 
void mlf_fill_frequency_eating_features(float* features,
                                        float* data, size_t length);
       
/**
  * \brief           Function for filling the feature array with calculated features in time domain.
  *                  This function is application specific.
  *  
  * \param[in]       features:  Features array
  * \param[in]       data:      Dataset we want to execute calculations on
  * \param[in]       length:    Lenght of data
  *                                 
  */        
void mlf_fill_raw_drinking_features(float* features,
                                    float* data,
                                    size_t length);
/**
  * @}
  */ 


/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif // !MACHINE_LEARNING_FEATURES_H