/*

*/
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <tgmath.h>

typedef double TCOEF;

//A separate method for deriving a qubic equation with complex coefficients
void print_equation(TCOEF a, TCOEF b, TCOEF c, TCOEF d)
{
    printf("(%.1lf)x^3 + (%.1lf)x^2 + (%.1lf)x + (%.1lf) = 0\n", a, b, c, d);
}

#define EPS 0.0000001 

bool isZero(TCOEF x){
	return x< EPS && x>EPS;
}

//A method that outputs a solution to a qubic equation
size_t cardano(TCOEF a, TCOEF b, TCOEF c, TCOEF d, TRES* xres)
{
    // to canonical form
    TCOEF a2 = a*a;
    TCOEF b2 = b*b;
    TCOEF a3 = a2*a;
    TCOEF b3 = b2*b;
    TCOEF ac = a*c;

    TCOEF p = (3*ac - b2)/(3*a2)/3;
    TCOEF q = (2*b3 - 9*ac*b+27*a2*d)/(27*a3)/2;

    TCOEF Q = p*p*p + q*q;
    TCOEF alpha = cbrt(-q + sqrt(Q));
    TCOEF beta = cbrt(-q - sqrt(Q));

    if (Q > EPS){
        TCOEF x1 = alpha + beta;
        TCOEF complex x2 = -(alpha+beta)/2 + ((alpha-beta)/2*sqrt(3))*I;
        TCOEF complex x3 = -(alpha+beta)/2 - ((alpha-beta)/2*sqrt(3))*I;
        TCOEF x = -b/3*a;
        printf("x1 = %lf\n", x1+x);
        printf("x2 = %lf + (%lf)i\n", creal(x2) + x, cimag(x2));
        printf("x3 = %lf + (%lf)i", creal(x3) + x, cimag(x3));
        xres[0] = x1+x + 0.0*I;
	xres[1] = creal(x2) + x + cimag(x2)*I;
	xres[2] = creal(x3) + x + cimag(x3)*I;
        return 3;
    }
    else if (Q < EPS){
        TCOEF pfi;
        if(q<0){
            pfi = atan(sqrt(-Q)/(-q));
        }
        else if (q>0){
            pfi = atan(sqrt(-Q)/(-q)) + M_PI;
        }
        else{
            pfi = M_PI/2;
        }
        TCOEF x1 = 2*sqrt(-p)*cos(pfi / 3);
        TCOEF x2 = 2*sqrt(-p)*cos(pfi / 3 + 2*M_PI / 3);
        TCOEF x3 = 2*sqrt(-p)*cos(pfi / 3 + 4*M_PI / 3);
        TCOEF x = -b/3*a;
        printf("x1 = %lf\n", x1+x);
        printf("x2 = %lf\n", x2+x);
        printf("x3 = %lf\n", x3+x);

        xres[0] = x1+x + 0.0*I;
	xres[1] = x2+x + 0.0*I;
	xres[2] = x3+x + 0.0*I;
        return 3;
    }

        TCOEF x1 = 2*cbrt(-q);
        TCOEF x2 = -1*cbrt(-q);
        TCOEF x = -b/3*a;
        printf("x1 = %lf\n", x1+x);
        printf("x2 = %lf\n", x2+x);

        xres[0] = x1+x + 0.0*I;
	xres[1] = x2+x + 0.0*I;
	return 2;

}

//A method that writes the solution of a qubic equation to a file
void print_solution_t_file(double a, double b, double c, double d, char filename[N])
{
    FILE *fout;
    fout = fopen(filename, "w");
    double p = (3*a*c-b*b)/(3*a*a);
    double q = (2*b*b*b-9*a*b*c+27*a*a*d)/(27*a*a*a);
    double Q = (p/3)*(p/3)*(p/3) + (q/2)*(q/2);
    double alpha = cbrt(-q/2 + sqrt(Q));
    double beta = cbrt(-q/2 - sqrt(Q));
    if (Q > 0){
        double x1 = alpha + beta;
        double complex x2 = -(alpha+beta)/2 + ((alpha-beta)/2*sqrt(3))*I;
        double complex x3 = -(alpha+beta)/2 - ((alpha-beta)/2*sqrt(3))*I;
        double x = -b/3*a;
        fprintf(fout, "x1 = %lf\n", x1+x);
        fprintf(fout, "x2 = %lf + (%lf)i\n", creal(x2) + x, cimag(x2));
        fprintf(fout, "x3 = %lf + (%lf)i", creal(x3) + x, cimag(x3));
    }
    if (Q < 0){
        double pfi;
        if(q<0){
            pfi = atan(sqrt(-Q)/(-q/2));
        }
        else if (q>0){
            pfi = atan(sqrt(-Q)/(-q/2)) + M_PI;
        }
        else{
            pfi = M_PI/2;
        }
        double x1 = 2*sqrt(-p/3)*cos(pfi / 3);
        double x2 = 2*sqrt(-p/3)*cos(pfi / 3 + 2*M_PI / 3);
        double x3 = 2*sqrt(-p/3)*cos(pfi / 3 + 4*M_PI / 3);
        double x = -b/3*a;
        fprintf(fout, "x1 = %lf\n", x1+x);
        fprintf(fout, "x2 = %lf\n", x2+x);
        fprintf(fout, "x3 = %lf\n", x2+x);
    }
    if (Q == 0){
        double x1 = 2*cbrt(-q/2);
        double x2 = -1*cbrt(-q/2);
        double x = -b/3*a;
        fprintf(fout, "x1 = %lf\n", x1+x);
        fprintf(fout, "x2 = %lf\n", x2+x);
    }
    fclose(fout);
}

//This method that gets four coefficients for equation
double *coefs_f_file(char filename[N])
{
    FILE *fin;
    fin = fopen(filename, "r");
    double a, b, c, d;
    static double mas[4];
    fscanf(fin,"%lf%lf%lf%lf", &a, &b, &c, &d);
    mas[0] = a; mas[1] = b; mas[2] = c; mas[3] = d;
    fclose(fin);
    return mas;
}

int main()
{
    int mode_input, mode_output;
    double a, b, c, d;
    printf("Choose the INPUT mode\n0 - Console\n1 - File\nYour choice: ");
    scanf("%d", &mode_input);
    printf("Choose the OUTPUT mode\n0 - Console\n1 - File\nYour choice: ");
    scanf("%d", &mode_output);
    if (mode_input == 0){
        printf("Enter fist coef of quadratic equation.\n");
        scanf("%lf", &a);
        printf("Enter second coef of quadratic equation.\n");
        scanf("%lf", &b);
        printf("Enter third coef of quadratic equation.\n");
        scanf("%lf", &c);
        printf("Enter fourth coef of quadratic equation.\n");
        scanf("%lf", &d);
        if (mode_output == 0){
            print_equation(a, b, c, d);
            solution(a, b, c, d);
        }
        else if (mode_output == 1){
             print_solution_t_file(a, b, c, d, "result1.txt");
        }
    }
    else if (mode_input == 1){
        char filename[N];
        printf("Input filename for coefs (recommend coefs.txt) : ");
        scanf("%99s", filename);
        double* coefs = coefs_f_file(filename);
        a = coefs[0]; b = coefs[1]; c = coefs[2]; d = coefs[3];
        if (mode_output == 0){
            print_equation(a, b, c, d);
            solution(a, b, c, d);
        }
        else if (mode_output == 1){
             print_solution_t_file(a, b, c, d, "result1.txt");
        }
    }
    return 0;
}
