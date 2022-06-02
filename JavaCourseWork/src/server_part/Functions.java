package server_part;
import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class Functions {
    public static < O > String prt(List<O> array ) {
        StringBuilder str = new StringBuilder();
        for (O obj : array) {
            str.append(obj).append(" ");
        }
        return str.toString();
    }

    public static ArrayList<String> graeffe(List<BigDecimal> given_coefs){

        int p_degree = given_coefs.size();
        List<BigDecimal> coefs = new ArrayList<>(given_coefs);

        MathContext mc = new MathContext(20);
        RoundingMode rm = RoundingMode.HALF_UP;

        if (!Objects.equals(given_coefs.get(0), BigDecimal.ONE)){
            for(int m = 0; m < p_degree; m++) {
                coefs.set(m, given_coefs.get(m).divide(given_coefs.get(0),mc));
            }
        }


        List<BigDecimal> cur_coefs = new ArrayList<>(coefs);
        List<BigDecimal> squared_coefs = new ArrayList<>(coefs);
        List<BigDecimal> bks = new ArrayList<>(coefs);

        List<BigDecimal> prob_roots = new ArrayList<>();
        List<BigDecimal> roots = new ArrayList<>();

        bks.set(0,BigDecimal.valueOf(0));
        bks.set(p_degree-1,BigDecimal.valueOf(0));
        BigDecimal aL;
        BigDecimal aR;
        BigDecimal cur_bk;
        int iteration = 0;

        for(int m = 0; m < p_degree+5; m++) {
            // squared coefs
            for(int i = 0; i < p_degree; i++) {
                squared_coefs.set(i,cur_coefs.get(i).pow(2));
            }

            // bk coefs
            for(int j = 1; j < p_degree-1; j++) {
                cur_bk = BigDecimal.valueOf(0);
                for (int h = 1; h<p_degree; h++){
                    if ((j-h==-1)|(j+h==p_degree)) break;
                    aL = cur_coefs.get(j-h);
                    aR = cur_coefs.get(j+h);
                    cur_bk = cur_bk.add(aL.multiply(aR).multiply(BigDecimal.valueOf(Math.pow(-1, h))).multiply(BigDecimal.valueOf(2)));
                }
                bks.set(j, cur_bk);
            }

            // new coefs
            for(int k = 0; k < p_degree; k++) {
                cur_coefs.set(k, squared_coefs.get(k).add(bks.get(k)));
            }
            iteration++;
        }

        List<BigDecimal> result_coefs = new ArrayList<>(cur_coefs);
        BigDecimal cur_root;
        BigDecimal cur_coef;
        BigDecimal prev_coef = BigDecimal.valueOf(1);

        for(int z = 1; z < p_degree; z++) {
            cur_coef = result_coefs.get(z);
            cur_root = cur_coef.divide(prev_coef, mc).abs();
            for (int w = 0; w<iteration;w++){
                cur_root = cur_root.sqrt(mc);
            }
            prob_roots.add(cur_root);
            prev_coef = cur_coef;
        }

        BigDecimal prob_resultP;
        BigDecimal prob_resultN;
        BigDecimal needed = BigDecimal.valueOf(0.0001);
        List<BigDecimal> prob_complex = new ArrayList<>(prob_roots);

        for (BigDecimal x: prob_roots){
            prob_resultP = BigDecimal.valueOf(0);
            prob_resultN = BigDecimal.valueOf(0);
            for (int g = 0; g<p_degree;g++){
                prob_resultP = prob_resultP.add(coefs.get(g).multiply(x.pow(p_degree-g)));
                prob_resultN = prob_resultN.add(coefs.get(g).multiply(x.multiply(BigDecimal.valueOf(-1)).pow(p_degree-g)));
            }

            if (prob_resultP.setScale(3, rm).equals(needed.setScale(3, rm))){
                roots.add(x);
                prob_complex.remove(x.abs());
            }
            else if (prob_resultN.setScale(3, rm).equals(needed.setScale(3, rm))){
                roots.add(x.multiply(BigDecimal.valueOf(-1)));
                prob_complex.remove(x);
            }

        }

        ArrayList<String> rootsList = new ArrayList<String>();
        for (BigDecimal x: roots){rootsList.add(x.setScale(5, rm).toString()+";");}

        if (prob_complex.size()!=0){
            if (prob_complex.size() == 2){
                BigDecimal xns = BigDecimal.valueOf(0);
                BigDecimal cos,sin,fi,real,img,a0,a1;
                BigDecimal p = prob_complex.get(0).add(prob_complex.get(1)).divide(BigDecimal.valueOf(2),mc);
                for (BigDecimal x: roots){xns = xns.add(x);}
                a0 = coefs.get(0);
                a1 = coefs.get(1);
                cos = xns.add(a1.divide(a0,mc)).multiply(BigDecimal.valueOf(-1));
                cos = cos.divide(BigDecimal.valueOf(2).multiply(p),mc);
                fi = BigDecimal.valueOf(Math.acos(cos.doubleValue()));
                sin = BigDecimal.valueOf(Math.sin(fi.doubleValue()));
                real = cos.multiply(p).setScale(5, rm);
                img = sin.multiply(p).setScale(5,rm);
                rootsList.add(real+ " + " + img + "i;");
                rootsList.add(real+ " - " + img + "i;");
            }
            else if (prob_complex.size() == 4){
                BigDecimal xns = BigDecimal.valueOf(0);
                BigDecimal re_xns = BigDecimal.valueOf(0);
                BigDecimal cos1,cos2,p1,p2,real1,real2,img1,img2,top2,bot2,a0,a1,an_1,an,fi1,fi2,sin1,sin2;
                for (BigDecimal x: roots){
                    re_xns = re_xns.add(BigDecimal.valueOf(1).divide((x),mc));
                    xns = xns.add(x);
                }

                a0 = coefs.get(0);
                a1 = coefs.get(1);
                an_1 = coefs.get(p_degree-2);
                an = coefs.get(p_degree-1);
                p1 = prob_complex.get(0).add(prob_complex.get(1)).divide(BigDecimal.valueOf(2),mc);
                p2 = prob_complex.get(2).add(prob_complex.get(3)).divide(BigDecimal.valueOf(2),mc);

                top2 = an_1.divide(an,mc).add(re_xns).multiply(BigDecimal.valueOf(-1));
                top2 = top2.multiply(p1).multiply(p1).multiply(p2);
                top2 = top2.add(p2.multiply(a1.divide(a0,mc).add(xns)));
                bot2 = p1.multiply(p1).add(BigDecimal.valueOf(-1).multiply(p2).multiply(p2));
                bot2 = BigDecimal.valueOf(2).multiply(bot2);
                cos2 = top2.divide(bot2,mc);
                fi2 = BigDecimal.valueOf(Math.acos(cos2.doubleValue()));
                sin2 = BigDecimal.valueOf(Math.sin(fi2.doubleValue()));

                real2 = p2.multiply(cos2).setScale(5, rm);
                img2 = p2.multiply(sin2).setScale(5, rm);

                cos1 = a1.divide(a0,mc).add(xns).divide(BigDecimal.valueOf(-2),mc);
                cos1 = cos1.add(p2.multiply(cos2).multiply(BigDecimal.valueOf(-1)));
                cos1 = cos1.divide(p1,mc);
                fi1 = BigDecimal.valueOf(Math.acos(cos1.doubleValue()));
                sin1 = BigDecimal.valueOf(Math.sin(fi1.doubleValue()));

                real1 = p1.multiply(cos1).setScale(5, rm);
                img1 = p1.multiply(sin1).setScale(5, rm);

                rootsList.add(real1+ " + " + img1 + "i;");
                rootsList.add(real1+ " - " + img1 + "i;");
                rootsList.add(real2+ " + " + img2 + "i;");
                rootsList.add(real2+ " - " + img2 + "i;");
            }

            else {
                for (BigDecimal r: prob_complex){
                    rootsList.add("r = "+r);
                }
            }
        }
        return rootsList;
    }
}
