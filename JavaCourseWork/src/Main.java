import java.util.Arrays;
import java.util.Scanner;
public class Main {
    public static String prt(double[] array){
        String str = "";
        for (int i = 0; i< array.length;i++){
            str = str + String.valueOf(array[i]) + " ";
        }
        return str;
    }
    public static double[] digit_count(double[] coefs){
        int p_degree = coefs.length;
        double[] cur_coefs = Arrays.copyOf(coefs, p_degree);;
        double[] squared_coefs = new double[p_degree];
        double[] v2 = new double[p_degree];
        v2[0] = 0;
        v2[p_degree-1] = 0;
        for(int m = 0; m < p_degree+1; m++) {
            // квадраты коефов
            for(int i = 0; i < p_degree; i++) {
                squared_coefs[i] = (double) Math.pow(cur_coefs[i],2);
            }

            System.out.println("Квадраты: "+prt(squared_coefs));

            // 2v
            for(int j = 1; j < p_degree-1; j++) {
                v2[j] = 2*cur_coefs[j-1]*cur_coefs[j+1];
            }

            System.out.println("2v: " + prt(v2));

            // new coefs
            for(int j = 0; j < p_degree; j++) {
//                System.out.println(" "+(float) Math.pow(-1,j+1) * v2[j]+" "+(float) Math.pow(-1,j) * squared_coefs[j]);
                cur_coefs[j] = (double) Math.pow(-1,j+1) * v2[j] + (double) Math.pow(-1,j) * squared_coefs[j];
            }

            System.out.println("Конец итерации: "+prt(cur_coefs));
        }
        return cur_coefs;
    }

    public static void main(String[] args){
//        Scanner sc = new Scanner(System.in);
//        System.out.println("Eneter polynomial degree");
//        int p_degree1 = sc.nextInt()+1;
//        float[] coefs1 = new float[p_degree1];
//        for(int i = 0; i < p_degree1; i++) {
//            coefs1[i] = sc.nextFloat();
//        }
//        double[] coefs1 = {1,-6,5,12};
//        System.out.println(prt(digit_count(coefs1)));
        System.out.println(Math.log(5) / Math.log(2)*5);

    }
}