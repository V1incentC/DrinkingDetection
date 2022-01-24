#include "machine_learning_features.h"
#include "mlf_windows.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include  "arm_math.h"

/* TIME DOMAIN FEATURES */

float mlf_mean_value(float* data, size_t length)
{
    float sum = 0;
    for (size_t i = 0; i < length; ++i)
    {
        sum = sum + data[i];
    }
    return (sum / length);
}


float mlf_standard_deviation(float* data, size_t length)
{
    float sum = 0;
    float mean_value = mlf_mean_value(data, length);

    for(size_t i = 0; i < length; ++i)
    {
        sum += (data[i] - mean_value) * (data[i] - mean_value);
    }
    return sqrt(sum/length);
}


float mlf_coefficient_of_variation(float* data, size_t length)
{
    return mlf_standard_deviation(data,length) / mlf_mean_value(data,length);
}


float mlf_abs_mean_value(float* data, size_t length)
{
    float sum = 0, temp = 0;

    for(size_t i = 0; i < length; ++i)
    {
        temp = data[i];
        sum += fabs(temp);
    }
    return (sum / length);
}


uint16_t mlf_num_of_zero_crossings(float* data, size_t length)
{
    uint16_t count = 0;

    for(size_t i = 0; i < length; ++i)
    {
        if(data[i] * data[i - 1] < 0)
        {
            ++count;
        }
    }
    return count;
}


float mlf_amplitude(float *data, size_t length)
{
    float max = data[0];
    float min = data[0];

    for (size_t i = 0; i < length; ++i)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
        if (data[i] < min)
        {
            min = data[i];
        }
    }
    return max - min;
}


float mlf_velocity(float* data, size_t length, float timestep)
{
    float velocity = 0.0, dT = 1.0;

    for(size_t i = 0; i < length; ++i)
    {
        velocity += data[i] * dT;
        dT = timestep;
    }
    return velocity;
}


float mlf_sum_per_component(float* data, size_t length, float timestep)
{
    float sum_per_component = 0, dT = 1.0;

    for(size_t i = 0; i < length; ++i)
    {
        sum_per_component += fabs(data[i]) * dT;
        if(i < (length - 1))
        {
            dT = timestep;
        }
    }
    return sum_per_component;
}


uint16_t mlf_mean_crossing_rate(float* data, size_t length)
{
    float mean = mlf_abs_mean_value(data,length);

    float last = data[0] - mean;
    float current = 0;
    uint16_t crossings = 0;

    for(size_t i = 0; i < length; ++i)
    {
        current = data[i] - mean;
        if(last * current < 0)
        {
            ++crossings;
        }
        last = current;
    }
    return crossings;
}


float mlf_entropy(float* data, size_t length)
{
    float entropy = 0, sum = 0, temp = 0;

    for(size_t i = 0; i < length; ++i)
    {
        sum += data[i];
    }

    for(size_t i = 0; i < length; ++i)
    {
        temp = data[i] / sum;

        if(temp > 0.0)
        {
            entropy -= temp * (float) log(temp);
        }
    }
    return entropy;
}


/* Support function for qsort */
int cmpfunc(const void * a, const void * b)
{
    return (*(float*)a >= *(float*)b) ? 1 : -1;
}


/* Function to give index of the median */
static int32_t median(float* a, int32_t l, int32_t r)
{
    int32_t n = r - l + 1; 
    n = (n + 1) / 2 - 1; 
    return n + l; 
}


float mlf_iqr(float* data, size_t length)
{
    float Q1, Q3;
    int32_t mid_index, second_index;

    float* temp_data = malloc(length * sizeof *temp_data);

    if(temp_data == NULL)
    {
        return EXIT_FAILURE;
    }
    else
    {
        /* Store the data in temp_data to preserve original order */
        memcpy(temp_data, data, length * sizeof(float));
        /* Sort the temporary data */
        qsort(temp_data, length, sizeof(float), cmpfunc);

        /* Index of median od entire data */
        mid_index = median(temp_data, 0, length);
        /* Median value of first half */
        Q1 = temp_data[median(temp_data, 0, mid_index)]; 

        /* Median index of second half */
        second_index = median(temp_data, mid_index + 1, length);
        /* Median value of seconf half */
        Q3 = temp_data[second_index];

        free(temp_data);
        /* return IQR */
        return (Q3 - Q1);
    }
}


float mlf_kurtosis(float* data, size_t length)
{
    float mean = mlf_mean_value(data, length);
    float kurtosis = 0, m2 = 0, m4 = 0;

    /* Calculate second and fourth moment around the mean */
    for(size_t i = 0; i < length; ++i)
    {
        m2 += (data[i] - mean) * ( data[i] - mean);
        m4 += (data[i] - mean) * (data[i] - mean) * (data[i] - mean) * (data[i] - mean);
    }

    m2 = m2 / length;
    m4 = m4 / length;
    kurtosis = m4 / pow(m2,2) - 3;

    return kurtosis;
}


float mlf_skewness(float* data, size_t length)
{
    float mean = mlf_mean_value(data, length);
    float skewness = 0, m2 = 0, m3 = 0;

    /* Calculate second and third moment around the mean */
    for(size_t i = 0; i < length; ++i)
    {
        m2 += (data[i] - mean) * ( data[i] - mean);
        m3 += (data[i] - mean) * (data[i] - mean) * (data[i] - mean);
    }

    m2 = m2 / length;
    m3 = m3 / length;
    skewness = m3 / pow(m2,1.5);

    return skewness;
}


float mlf_absolute_sum_of_changes(float* data, size_t length)
{
    float sum = 0;
    for (size_t i = 0; i < length - 1; ++i)
        sum += fabs(data[i] - data[i+1]);
    return sum;
}


uint16_t mlf_count_above_mean(float* data, size_t length)
{
    float mean = mlf_mean_value(data, length);
    uint16_t count = 0;
    for(size_t i = 0; i < length; ++i)
    {
        if(data[i] > mean)
            ++count;
    }
    return count;
}


uint16_t mlf_count_below_mean(float* data, size_t length)
{
    float mean = mlf_mean_value(data, length);
    uint16_t count = 0;
    for(size_t i = 0; i < length; ++i)
    {
        if(data[i] < mean)
            ++count;
    }
    return count;
}


float mlf_maximum(float* data, size_t length)
{
    float *maximum = data;
    *maximum = data[0];

    for(size_t i = 1; i < length; ++i)
    {
        if(data[i] > *maximum)
        {
            *maximum = data[i];
        }
    }
    return *maximum;
}


float mlf_minimum(float* data, size_t length)
{
    float *minimum = data;
    *minimum = data[0];

    for(size_t i = 1; i < length; ++i)
    {
        if(data[i] < *minimum)
        {
            *minimum = data[i];
        }
    }
    return *minimum;
}


float mlf_median(float* data, size_t length)
{
    float* temp_data = malloc(length * sizeof *temp_data);
    float median_value;
    uint16_t index;

    if(temp_data == NULL)
    {
        return EXIT_FAILURE;
    }
    else
    {
        /* Store the data in temp_data to preserve original order */
        memcpy(temp_data, data, length * sizeof(float));
        /* Sort the temporary data */
        qsort(temp_data, length, sizeof(float), cmpfunc);

        /* Index of median of entire data */
        index = median(temp_data, 0, length);

        if(index % 2 == 0)
        {
            median_value = (temp_data[index - 1] + temp_data[index]) / 2.0;
            free(temp_data);

            return median_value;
        }
        else
        {
            median_value = temp_data[index];
            free(temp_data);

            return median_value;
        }
    }
}


float mlf_mean_abs_change(float* data, size_t length)
{
    float sum = 0;
    for(size_t i = 0; i < length - 1; ++i)
    {
        sum += fabs(data[i] - data[i + 1]);
    }
    return sum / (length - 1);
}


float mlf_mean_change(float* data, size_t length)
{
    float sum = 0;
    for(size_t i = 0; i < length - 1; ++i)
    {
        sum += data[i] - data[i + 1];
    }
    return sum / (length - 1);
}


uint16_t mlf_longest_strike_above_mean(float *data, size_t length)
{
    if(length <= 0) return 0;

    float mean = mlf_mean_value(data, length);
    uint16_t count = 0, longest_strike = 0;

    for(size_t i = 0; i < length; ++i)
    {
        if(data[i] > mean)
        {
            ++count;
            if(count > longest_strike)
            {
                longest_strike = count;
            }
        }
        else
        {
            count = 0;
        }
            
    }
    return longest_strike;
}


uint16_t mlf_longest_strike_below_mean(float *data, size_t length)
{
    if(length <= 0) return 0;

    float mean = mlf_mean_value(data, length);
    uint16_t count = 0, longest_strike = 0;

    for(size_t i = 0; i < length; ++i)
    {
        if(data[i] < mean)
        {
            ++count;
            if(count > longest_strike)
            {
                longest_strike = count;
            }
        }
        else
        {
            count = 0;
        }
            
    }
    return longest_strike;
}


/* FREQUENCY DOMAIN FEATURES */

 
void mlf_sample_frequencies(uint16_t sampling_frequency,
                            uint16_t length_of_fft,
                            uint16_t length_of_output_array,
                            float* output_array)
{
    for(uint16_t i = 0; i < length_of_output_array; ++i)
    {
        output_array[i] = (float) (i * sampling_frequency) / length_of_fft;
    }

}


uint8_t mlf_fft_numpy(float* data_in,    size_t data_in_length,
                      float* data_out, uint32_t data_out_length)
{
  
    float* temp_data_in = malloc(data_in_length * sizeof *temp_data_in);
    float* temp_data_out = malloc(data_out_length * sizeof *temp_data_out);
    if ((temp_data_in == NULL) || (temp_data_out == NULL))
    {
        return EXIT_FAILURE;
    }
    else
    {
        /* Store the data in temp_data to preserve original order */
        memcpy(temp_data_in, data_in, data_in_length * sizeof(float));
        
        
        /* variables needed for arm_rfft */
        uint32_t fftSize      = data_in_length;
        uint32_t ifftFlag     = 0;
        uint32_t doBitReverse = 1;
        arm_rfft_fast_instance_f32 varInstRfftF32;
        arm_status status;
        status = ARM_MATH_SUCCESS;

        status = arm_rfft_fast_init_f32(&varInstRfftF32, fftSize);
        if (status != ARM_MATH_SUCCESS)
        {
            return EXIT_FAILURE;
        }
        
        arm_rfft_fast_f32(&varInstRfftF32, temp_data_in, temp_data_out, ifftFlag);
        
        /* Programming magic I did a year ago that will
         * not be reafactored because I don't undrestand it*/
        
        float temp;
        temp = temp_data_out[1]; /* we need to save the second element */
        temp_data_out[1] = 0; /* set second element to 0 because the DC component of fft can't have an imaginary part */
  
        /* moving the second element to the middle of output array and setting the +1 element to 0 */
        temp_data_out[data_in_length] = temp;
        temp_data_out[data_in_length + 1] = 0.0f;

        for (size_t i = 0; i < data_out_length; i += 2)
        {
            /* fil the output_arr with input values */
            *(data_out + i / 2) = *(temp_data_out + i / 2);
    
            /* flip the array values and skip the middle element */
            if (i  < (data_in_length + 1))
            {
                /* take the elements */
                *(data_out + data_out_length - i) = *(temp_data_out + i);
                *(data_out + data_out_length - i + 1) = -1 * *(temp_data_out + i + 1);
            }
        }
        
        free(temp_data_in);
        free(temp_data_out);
        
        return EXIT_SUCCESS;
    }
    

}

static inline float absolute(float a, float b)
{
    return sqrt((a*a + b*b));
}
static void  absolute_array(float* data,     size_t length,
                             float* data_out, size_t length_out)
{
    for (size_t i = 0; i < length; i += 2)
    {
        data_out[i / 2] = absolute(data[i], data[i + 1]);
    }
}
    
static inline float square(float a)
{
    return a*a;
}

static void square_array(float* data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        data[i] = square(data[i]);
    }
}

static float sum_array (float* data, size_t length)
{
    float sum = 0;
    for (size_t i = 0; i < length; ++i)
    {
        sum += data[i];
    }
    return sum;
}


float mlf_spectrum_entropy(float* data, size_t length)
{
    float entropy;
    float* temp_data = malloc(length/2 * sizeof *temp_data);
      
    if (temp_data == NULL)
    {
        return EXIT_FAILURE;
    }
    else
    {
        absolute_array(data, length, temp_data, length / 2);
        entropy = mlf_entropy(temp_data, length / 2);
        
        free(temp_data);
        
        return entropy;
    }
}


float mlf_spectrum_energy(float* data, size_t length)
{
    float energy;
    float* temp_data = malloc(length / 2 * sizeof *temp_data);
    
    if (temp_data == NULL)
    {
        return EXIT_FAILURE;
    }
    else
    {
        absolute_array(data, length, temp_data, length / 2);
        square_array(temp_data, length / 2);
        energy = sum_array(temp_data, length / 2) / length * 2;
        
        free(temp_data);
        return energy;
    }
}