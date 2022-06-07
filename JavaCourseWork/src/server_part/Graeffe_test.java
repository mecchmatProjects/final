package server_part;

import java.math.*;

import java.util.ArrayList;
import java.util.List;

import static server_part.Functions.graeffe;
import static server_part.Functions.prt;



public class Graeffe_test {

    public static void main(String[] args){
//        Scanner sc = new Scanner(System.in);
//        System.out.println("Eneter polynomial degree");
//        int p_degree1 = sc.nextInt()+1;
//        float[] coefs1 = new float[p_degree1];
//        for(int i = 0; i < p_degree1; i++) {
//            coefs1[i] = sc.nextFloat();
//        }
        List<BigDecimal> lister = new ArrayList<>();
        double[] coefs1 = {1,1,1,1};
        for (double item : coefs1) {
            lister.add(new BigDecimal(item));
        }
        System.out.println("Roots: " + prt(graeffe(lister)));
    }
}