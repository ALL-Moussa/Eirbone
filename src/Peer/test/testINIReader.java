package project;

public class testINIReader {
    public static void main(String[] args) {
        System.out.println("Testing INI Reader..");

        INIReader r = new INIReader("Peer/config.ini");
        String[] ips = r.getIPs();
        int[] ports = r.getPorts();
        int ps = r.getPieceSize();
        assert "127.0.0.1".equals(ips[0]) : "Erreur dans la valeur de tracker-address";
        assert "192.162.1.1".equals(ips[1]) : "Erreur dans la valeur de peer-address";
        assert ports[0] == 80 : "Erreur dans la valeur de tracker-port";
        assert ports[1] == 443 : "Erreur dans la valeur de peer-port";
        assert ps == 1024 : "Erreur dans la valeur de piece-size";
        System.out.println("INI Reader OK!");
        
    }
}
