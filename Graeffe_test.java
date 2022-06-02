import java.math.*;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class Graeffe_test {
    public static String prt(List<BigDecimal> array){
        StringBuilder str = new StringBuilder();
        for (int i = 0; i< array.size();i++){
            str.append(array.get(i)).append(" ");
        }
        return str.toString();
    }
    public static List<BigDecimal> Graeffe(List<BigDecimal> given_coefs){
        List<BigDecimal> coefs = new ArrayList<BigDecimal>(given_coefs);
        int p_degree = coefs.size();
        MathContext mc = new MathContext(10);

        if (!Objects.equals(given_coefs.get(0), BigDecimal.ONE)){
            for(int m = 0; m < p_degree; m++) {
                coefs.set(m, given_coefs.get(m).divide(given_coefs.get(0),mc));
            }
        }

        RoundingMode rm = RoundingMode.HALF_UP;
        List<BigDecimal> cur_coefs = new ArrayList<BigDecimal>(coefs);
        List<BigDecimal> squared_coefs = new ArrayList<BigDecimal>(coefs);
        List<BigDecimal> bks = new ArrayList<BigDecimal>(coefs);
        List<BigDecimal> prob_answers = new ArrayList<BigDecimal>();
        List<BigDecimal> answers = new ArrayList<BigDecimal>();
        bks.set(0,BigDecimal.valueOf(0));
        bks.set(p_degree-1,BigDecimal.valueOf(0));
        BigDecimal a1;
        BigDecimal a2;
        BigDecimal cur_bk;
        int iteration = 0;
        // Math.log(x) / Math.log(b)

        for(int m = 0; m < p_degree+5; m++) {
            // квадраты коефов
            for(int i = 0; i < p_degree; i++) {
                squared_coefs.set(i,cur_coefs.get(i).pow(2));
            }

//            System.out.println("Квадраты: "+ prt(squared_coefs));

            // bk coefs
            for(int j = 1; j < p_degree-1; j++) {
                cur_bk = BigDecimal.valueOf(0);
                for (int h = 1; h<p_degree; h++){
                    if ((j-h==-1)|(j+h==p_degree)) break;
                    a1 = cur_coefs.get(j-h);
                    a2 = cur_coefs.get(j+h);
                    cur_bk = cur_bk.add(a1.multiply(a2).multiply(BigDecimal.valueOf(Math.pow(-1, h))).multiply(BigDecimal.valueOf(2)));
                }
                bks.set(j, cur_bk);
            }

//            System.out.println("bk: " + prt(bks));

            // new coefs
            for(int k = 0; k < p_degree; k++) {
//                System.out.println(" "+(float) Math.pow(-1,j+1) * v2[j]+" "+(float) Math.pow(-1,j) * squared_coefs[j]);
                cur_coefs.set(k, squared_coefs.get(k).add(bks.get(k)));
            }

            iteration++;
            System.out.println("Конец итерации: "+prt(cur_coefs));
        }
        System.out.println("Конец итерациий: "+ prt(cur_coefs));
        List<BigDecimal> result_coefs = new ArrayList<BigDecimal>(cur_coefs);
        BigDecimal cur_root;
        BigDecimal cur_coef;
        BigDecimal prev_coef = BigDecimal.valueOf(1);

        for(int z = 1; z < p_degree; z++) {
            cur_coef = result_coefs.get(z);
            cur_root = cur_coef.divide(prev_coef, mc).abs();
            for (int w = 0; w<iteration;w++){
                cur_root = cur_root.sqrt(mc);
            }
            prob_answers.add(cur_root);

            prev_coef = cur_coef;
        }
        System.out.println(prt(prob_answers));
        BigDecimal prob_resultP;
        BigDecimal prob_resultN;
        BigDecimal needed = BigDecimal.valueOf(0.0001);
        List<BigDecimal> results = new ArrayList<BigDecimal>();
        for (BigDecimal x: prob_answers){

            prob_resultP = BigDecimal.valueOf(0);
            prob_resultN = BigDecimal.valueOf(0);
            for (int g = 0; g<p_degree;g++){
                prob_resultP = prob_resultP.add(coefs.get(g).multiply(x.pow(p_degree-g)));
            }
            for (int g = 0; g<p_degree;g++){
                prob_resultN = prob_resultN.add(coefs.get(g).multiply(x.multiply(BigDecimal.valueOf(-1)).pow(p_degree-g)));
            }

            if (prob_resultP.setScale(3, rm).equals(needed.setScale(3, rm))){
                results.add(x);
            }
            if (prob_resultN.setScale(3, rm).equals(needed.setScale(3, rm))){
                results.add(x);
            }

        }
        return results;
    }

    public static void main(String[] args){
//        Scanner sc = new Scanner(System.in);
//        System.out.println("Eneter polynomial degree");
//        int p_degree1 = sc.nextInt()+1;
//        float[] coefs1 = new float[p_degree1];
//        for(int i = 0; i < p_degree1; i++) {
//            coefs1[i] = sc.nextFloat();
//        }
        List<BigDecimal> lister = new ArrayList<BigDecimal>();
        double[] coefs1 = {1, -2,0, 1, -1.5};
        for (double item : coefs1) {
            lister.add(new BigDecimal(item));
        }
        System.out.println("Целочисленные корни: " + prt(Graeffe(lister)));
    }
}