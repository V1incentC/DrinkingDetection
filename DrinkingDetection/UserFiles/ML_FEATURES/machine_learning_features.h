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
/**
  * @defgroup    MACHINE_LEARNING_FEATURES
  * @brief       This file contains function for calculating features on IMU signals in time and 
  *              frequency domain. Features are used for a machine learning model that predicts
  *              drinking based on calculated features from this file.
  * @{
  *
  */




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
  * \param[in]       data_in_length:    Length of inoput array
  * \param[in]       data_out:          Pointer to output array
  * \param[in]       data_out_length:   Length of output array
  *
  */        
uint8_t mlf_fft_numpy(float* data_in,    size_t data_in_length,
                      float* data_out, uint32_t data_out_length);


      
float mlf_spectrum_entropy(float* data, size_t length);
      
      
float mlf_spectrum_energy(float* data, size_t length);
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