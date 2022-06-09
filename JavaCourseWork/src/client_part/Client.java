package client_part;

import org.json.simple.JSONObject;

import java.io.*;
import java.math.BigDecimal;
import java.net.Socket;
import java.util.*;

import static server_part.Functions.*;

public class Client {

    private static Socket server;

    public static void main(String[] args) throws IOException, ClassNotFoundException {

        server = new Socket("localhost", 8080);

        BufferedReader in = new BufferedReader(new InputStreamReader(server.getInputStream()));
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(server.getOutputStream()));

        ObjectOutputStream outObj = new ObjectOutputStream(server.getOutputStream());
        ObjectInputStream inObj = new ObjectInputStream(server.getInputStream());

        ClientHttpManager httpManager = new ClientHttpManager(server);

        Scanner sc = new Scanner(System.in);

        String outputStr;
        String inputStr;

        ArrayList<String> massage = new ArrayList<String>();
        ArrayList<String> rootsList = new ArrayList<String>();

        List<BigDecimal> pol_coefs;
        Map<String, String> response;
        String user, reply,answer;

        int size;
        JSONObject json;
        Object Obj;
        String path,curPath;

        while (true) {
            System.out.println("Enter your login:");
            user = sc.nextLine();
            out.write(httpManager.POST("login", "txt", "txt", user));
            out.flush();
            massage.clear();
            System.out.println();
            while (true) {
                inputStr = in.readLine();
                if (inputStr.equals("")) break;
                massage.add(inputStr);
            }
            response = readHttp(massage);
            reply = response.get("Reply");
            if(reply.equals("Success")) {
                System.out.println("Welcome, " + user + "\n\r");
                break;
            }
            else {
                System.out.println(reply);
            }
        }


        while (true){
            pol_coefs = enterPolinom();
            outputStr = httpManager.POST("getRoots","txt","txt",pol_coefs);
            out.write(outputStr);
            out.flush();
            massage.clear();

            while (true) {
                inputStr = in.readLine();
                if (inputStr.equals("")) break;
                massage.add(inputStr);
            }
            response = readHttp(massage);
            reply = response.get("Reply");
            if(reply.equals("Success")) {
                rootsList.clear();
                size = Integer.parseInt(response.get("Size"));
                for(int i = 0; i < size; i++){
                    rootsList.add(response.get("x"+i));
                }
                System.out.println(prt(rootsList));
            }
            else {
                System.out.println(reply);
            }

            System.out.println("Get roots in json?(y/n)");
            answer = sc.nextLine();
            if (answer.equals("y")){
                outputStr = httpManager.GET("getJson","json","txt");
                out.write(outputStr);
                out.flush();

                Obj = inObj.readObject();
                json = (JSONObject) Obj;

                curPath = System.getProperty("user.dir");
                File theDir = new File(curPath+"/SavedJsonFiles");
                theDir.mkdirs();

                System.out.println("Enter filename: ");
                path = curPath+"\\SavedJsonFiles\\"+sc.nextLine()+".json";

//                PrintWriter outJson = new PrintWriter(new FileWriter(path));
//
//                outJson.write(json.toJSONString());
//                json.writeJSONString(outJson);

                FileWriter file = new FileWriter(path);
                file.write(json.toJSONString());
                file.close();

                System.out.print("Roots: ");
                rootsList = (ArrayList<String>) json.get("roots");
                System.out.println(rootsList);
                System.out.println("Roots were saved to: "+path);
            }
            else {
                outputStr = httpManager.POST("nextPolinom","txt","txt");
                out.write(outputStr);
                out.flush();
            }
        }
    }
}
