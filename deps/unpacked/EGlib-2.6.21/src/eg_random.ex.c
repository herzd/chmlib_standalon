/***********************************************************************\
 *
 * File:           EGrandState_t*.c for multiple streams of Random Numbers
 * Language:       ANSI C
 * Copyright:      Pierre L'Ecuyer, University of Montreal
 * Notice:         This code can be used freely for personal, academic,
 *                 or non-commercial purposes. For commercial purposes, 
 *                 please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca
 * Date:           14 August 2001
 *
\***********************************************************************/
/* Minor modifications by Daniel Espinoza to include it in EGlib 
 * - 07082006 */
/** @file
 * @ingroup EGrnd */
/** @addtogroup EGrnd */
/** @{ */
#include "EGlib.h"
#define sample 10000000L

int main (void)
{
	double sum = 0.0, sum3 = 0.0, sum2 = 0.0, sum4 = 0.0, kurtosis = 0.0,skewness = 0.0;
	long sumi = 0;
	int  i,j,rval=0;
	EGrandState_t g1=EGrandStateZero, g2=EGrandStateZero, g3=EGrandStateZero;
	EGrandState_t gar[4];
	unsigned long germe[6] = { 1, 1, 1, 1, 1, 1 };
	unsigned long curseed[6] = {0,0,0,0,0,0};
	EGlib_info();
	EGlib_version();
	/* set signal and limits */
	EGsigSet(rval,CLEANUP);
	EGsetLimits(3600.0,4294967295UL);
	EGrandInit(&g1);
	EGrandInit(&g2);
	EGrandInit(&g3);
	printf("Initial states of g1, g2, and g3:\n\n");
	EGrandGetState(&g1,curseed);
	printf("g1: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	EGrandGetState(&g2,curseed);
	printf("g2: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	EGrandGetState(&g3,curseed);
	printf("g3: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	sum = EGrandU01(&g2) + EGrandU01(&g3);
	for (i = 0;  i < 12345; i++) EGrandU01(&g2);

	printf ("State of g1 after advancing by 2^5 + 3 = 35 steps:\n");
	EGrandAdvanceState (&g1, 5L, 3L);   
	EGrandGetState(&g1,curseed);
	printf("g1: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	printf ("EGrandU01 (g1) = %12.8f\n", EGrandU01(&g1));
	printf ( "State of g1 after reset and advancing 35 times by 1:\n");
	EGrandReset(&g1);
	for (i = 0;  i < 35; i++) EGrandAdvanceState(&g1,0L,1L);
	EGrandGetState(&g1,curseed);
	printf("g1: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	printf ( "RandU01 (g1) = %12.8f\n\n", EGrandU01(&g1));
	EGrandReset(&g1);
	for (i = 0;  i < sample; i++)    sumi += EGrandInt(&g1, 1, 10);
	printf ( "State of g1 after reset and %ld calls to RandInt (1, 10):\n",sample);
	EGrandGetState(&g1,curseed);
	printf("g1: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	printf ( "   sum of %ld integers in [1, 10] = %ld\n\n", sample,sumi);
	sum += sumi/sample;
	printf ( "RandU01 (g1) = %12.8f\n\n", EGrandU01(&g1));
	EGrandReset(&g1);
	sum3 = 0.0;
	printf ( "Sum of first 100 output values from stream g3:\n");
	for (i = 0;  i < 100;  i++) {
		sum3 += EGrandU01(&g3);
	}
	printf ( "   sum = %12.6f\n\n", sum3);
	sum += sum3 / 10.0;
	printf ( "\nReset stream g3 to its initial seed.\n");
	EGrandReset(&g3);
	printf ( "First 5 output values from stream g3:\n");
	for (i=1; i<=5; i++)
		printf ("%12.8f\n", EGrandU01(&g3));
	sum += EGrandU01(&g3);
	printf (" Sum of %ld values from stream g2: ",sample);
	sum3 = 0.0;
	for (i=1; i<=sample; i++)
		sum3 += EGrandU01(&g2);
	printf ("%12.4f\n", sum3);
	sum += sum3/sample;
	printf (" Sum of %ld values from stream g3: ", sample);
	sum3 = 0.0;
	for (i=1; i<=sample; i++)
		sum3 += EGrandU01(&g3);
	printf ("%12.4f\n", sum3);
	sum += sum3/sample;

	printf ("\nSetPackageSeed to seed = { 1, 1, 1, 1, 1, 1 }\n");
	EGrandSetGlobalSeed(germe);

	printf ("\nCreate an array of 4 streams\n");
	EGrandInit(gar);
	EGrandInit(gar+1);
	EGrandInit(gar+2);
	EGrandInit(gar+3);

	for(i=0 ;  i < 4 ; i++)
	{
		sum3 = 0;
		for( j = sample ; j-- ; )
			sum3 += EGrandU01(gar+i);
		printf("sum %ld values from g[%d] = %12.6lf\n",sample,i,sum3);
	}

	printf ("Jump stream g[2] by 2^127 steps backward\n");
	EGrandAdvanceState (gar+2, -127L, 0L);
	EGrandGetState(gar+1,curseed);
	printf("g[1]: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	EGrandGetState(gar+2,curseed);
	printf("g[2]: "); for(i=0;i<6;i++){ printf("%lu ",curseed[i]);} printf("\n");
	for  (i = 0; i < 4; i++)
		sum += EGrandU01(gar+i);

	printf ("--------------------------------------\n");
	printf ("Final Sum = %.6f\n\n", sum);

	EGrandReset(gar);
	sum = 0.0;
	for(i=sample;i--;)
		sum += EGrandNormal(gar,202.0,3.15);
	sum /= sample;
	EGrandReset(gar);
	sum2 = 0.0;
	for(i=sample;i--;)
		sum2 += pow(EGrandNormal(gar,202.0,3.15)-sum,2.0);
	sum2 /= sample;
	EGrandReset(gar);
	sum3 = 0.0;
	for(i=sample;i--;)
		sum3 += pow(EGrandNormal(gar,202.0,3.15)-sum,3.0);
	sum3 /= sample;
	EGrandReset(gar);
	sum4 = 0.0;
	for(i=sample;i--;)
		sum4 += pow(EGrandNormal(gar,202.0,3.15)-sum,4.0);
	sum4 /= sample;
	EGrandAdvanceState(gar,0L,-2*sample);
	kurtosis = sum4/(sum2*sum2)-3.0;
	skewness = sum3/sqrt(sum2*sum2*sum2);
	printf("Normal(202,3.15)  with %ld observations:\n",sample);
	printf("\tkurtosis = %lf\n\tskewness = %lf\n",kurtosis,skewness);
	printf("\tm1 = %lg\n",sum);
	printf("\tm2 = %lg\n",sum2);
	printf("\tm3 = %lg\n",sum3);
	printf("\tm4 = %lg\n",sum4);

	sum = 0.0;
	for(i=sample;i--;)
		sum += EGrandExp(gar,3.5);
	sum /= sample;
	EGrandAdvanceState(gar,0L,-sample);
	sum2 = 0.0;
	for(i=sample;i--;)
		sum2 += pow(EGrandExp(gar,3.5)-sum,2.0);
	sum2 /= sample;
	EGrandAdvanceState(gar,0L,-sample);
	sum3 = 0.0;
	for(i=sample;i--;)
		sum3 += pow(EGrandExp(gar,3.5)-sum,3.0);
	sum3 /= sample;
	EGrandAdvanceState(gar,0L,-sample);
	sum4 = 0.0;
	for(i=sample;i--;)
		sum4 += pow(EGrandExp(gar,3.5)-sum,4.0);
	sum4 /= sample;
	EGrandAdvanceState(gar,0L,-sample);
	kurtosis = sum4/(sum2*sum2)-3.0;
	skewness = sum3/sqrt(sum2*sum2*sum2);
	printf("Exponential(3.5)  with %ld observations:\n",sample);
	printf("\tkurtosis = %lf\n\tskewness = %lf\n",kurtosis,skewness);
	printf("\tm1 = %lg\n",sum);
	printf("\tm2 = %lg\n",sum2);
	printf("\tm3 = %lg\n",sum3);
	printf("\tm4 = %lg\n",sum4);
	CLEANUP:
	return 0;
}
/** @} */
