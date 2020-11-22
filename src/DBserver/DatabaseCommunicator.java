package DBserver;
import java.sql.*;
/**
 *
 */
final class DatabaseCommunicator {
    private DatabaseCommunicator() {
    }

    private final static String localHostUrl = "jdbc:postgresql://127.0.0.1/Cybercom";

    static void saveMessage(final int packageID, final String message) throws SQLException {
        final Connection conn = establishConnection();
        final String query = "INSERT INTO Messages VALUES (?,?)";
        PreparedStatement ps = conn.prepareStatement(query);
        ps.setInt(1, packageID);
        ps.setString(2, message.replace("\0", "")); // replaces null terminated character to support format in DB.
        ps.execute();
        conn.close();
    }
    static String getMessage(final int packageID) throws SQLException {
        String message = "";
        final String query = "SELECT * FROM Messages WHERE packetID= ?";
        final Connection conn = establishConnection();
        PreparedStatement ps = conn.prepareStatement(query);
        System.out.println(packageID);
        ps.setInt(1, packageID);
        ResultSet rs = ps.executeQuery();

        while (rs.next()) {
            message = rs.getString("message");
        }
        conn.close();
        return message;
    }
    private static Connection establishConnection() throws SQLException {
        return DriverManager.getConnection(localHostUrl, "postgres", "123456");
    }

}
