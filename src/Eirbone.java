import java.util.ArrayList;
import java.util.List;

import project.Peer;

public class Eirbone {

    static class CmdOps{
        private String cOp = "config.ini";
        private List<String> fOp = new ArrayList<>();

        public CmdOps(String[] args) {
            if (args.length < 2 || !args[0].equals("-c")) {
                printHelp();
            } else {
                cOp = args[1];
                if (args.length > 2) {
                    for (int i = 3; i < args.length; i++) {
                        fOp.add(args[i]);
                    }
                } else {
                    String f = "files/file.txt";
                    fOp.add(f);
                }
            }
        }
        public String getConf(){
            return cOp;
        }
        public String[] getFiles(){
            return fOp.toArray(new String[0]);
        }
    }

    private static void printHelp() {
        // Print help message
        System.out.println("Usage: java Eirbone [-c] [-f]");
        System.out.println("Options:");
        System.out.println("  -c      : Config file path (config.ini if not specified)");
        System.out.println("  -f      : Files Folder path (files/ if not specified)");
    }

    private List<String> downloading;
    private List<String> leeching;
    

    private static void printProgress(Peer peer){

    }


    public static void main(String args[]){
        CmdOps cmd = new CmdOps(args);
        String configFile = cmd.getConf();
        String[] files = cmd.getFiles();
        Peer peer = new Peer(configFile, files);
        System.out.println("Annonce de presence..");
        String response_ann = peer.Announce();
        if(response_ann.equals("ok")){
            System.out.println("Annonce reussie");
        }else{
            System.out.println("Annonce a echoue");
            return;
        }
        printProgress(peer);
        
    }
    
}
