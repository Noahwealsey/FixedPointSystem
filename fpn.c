#include <stdio.h>
#include <stdint.h>

#define FP_INT_TYPE int32_t
typedef FP_INT_TYPE fp_int_t;

#define FP_INT_XL_TYPE int64_t
typedef FP_INT_XL_TYPE fp_int_xl_t;

#define FP_NUM_BITS         (sizeof(fp_int_t) << 3)
#define FP_INT_BITS         (14)
#define FP_FRAC_BITS        (FP_NUM_BITS - FP_INT_BITS)

#define FP_SCALE            (FP_FRAC_BITS)
#define FP_SCALE_FACTOR     (1 << FP_FRAC_BITS)

#define FP_INT_MASK         (((1 << FP_INT_BITS) -1) << FP_FRAC_BITS)
#define FP_FRAC_MASK        ((1 << FP_FRAC_BITS) -1 )

#define FP_PUSH_INT(a)      ((a) << FP_SCALE)
#define FP_VAL_0            0
#define FP_VAL_POS          FP_PUSH_INT(1)
#define FP_VAL_NEG          FP_PUSH_INT(-1) 
#define FP_VAL_HALF         (1 << (FP_FRAC_BITS - 1))   

#define FP_SIGN_BIT(a)      (((a) >> (FP_NUM_BITS - 1)) & 1)
#define FP_SIGN(a)          ((FP_SIGN_BIT(a) == 0 ? FP_VAL_POS : FP_VAL_NEG))
    
fp_int_t float_to_fp(float a){
    return (fp_int_t)(a * FP_SCALE_FACTOR);
}

float fp_to_float(fp_int_t a){
    return (float)a / FP_SCALE_FACTOR;
}

fp_int_t fp_div(fp_int_t a, fp_int_t b){
    return ((fp_int_xl_t)a << FP_FRAC_BITS) / b;
}

fp_int_t fp_mul(fp_int_t a, fp_int_t b){
    return ((fp_int_xl_t)a * b) >> FP_FRAC_BITS;
}

fp_int_t fp_abs(fp_int_t a){
    if(FP_SIGN_BIT(a) == 0){
        return a;
    }else{
        return ~a + 1;
    }
}

fp_int_t fp_frac(fp_int_t a){
    return fp_abs(a) & FP_FRAC_MASK;
}

fp_int_t fp_floor(fp_int_t a){
    if(fp_frac(a) == 0){
        return a;
    }
    if(FP_SIGN_BIT(a) == 0){
        return FP_INT_MASK & a;
    }else{
        return (FP_INT_MASK & a) + FP_VAL_POS;
    }
}

fp_int_t fp_ceil(fp_int_t a){
    if(fp_frac(a) == 0){
        return a;
    }
    else{
        return fp_floor(a) + FP_SIGN(a);
    }
}

fp_int_t fp_round(fp_int_t a){
    fp_int_t frac = fp_frac(a);
    if(frac >= FP_VAL_HALF){
        return fp_ceil(a);
    }else{
        return fp_floor(a);
    }
}

int main(){
    fp_int_t a = float_to_fp(12.5);
    
    const fp_int_t ref_voltage = float_to_fp(3.3f);
    const fp_int_t adc_max_volt = FP_PUSH_INT(4095);
    int32_t adc_reading = 2069;

    fp_int_t adc_fp = float_to_fp(adc_reading);
    fp_int_t result_voltage = fp_mul(ref_voltage, fp_div(adc_fp, adc_max_volt));
    printf("Resulting voltage is %fV\n", fp_to_float(result_voltage));

    return 0;
}