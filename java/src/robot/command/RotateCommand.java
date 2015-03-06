package robot.command;

public class RotateCommand implements Command
{
    private String direction;
    private byte magnitude;

    public RotateCommand(String direction, int magnitude)
    {
        this.direction = direction;
        this.magnitude = (byte) magnitude;
    }
    
    @Override
    public byte[] generatePayload()
    {
        byte buffer[] = new byte[8];

        //////////////////////////////////////// 
        // magic bytes
        buffer[0] = 0x47;
        buffer[1] = 0x41;
        buffer[2] = 0x43;
        buffer[3] = 0x53;
        
        //////////////////////////////////////// 
        // command type
        buffer[4] = 3;

        //////////////////////////////////////// 
        // velocity vector
        buffer[5] = 0;
        buffer[6] = 0;
        buffer[7] = 0;
        
        if (direction.equalsIgnoreCase("CW") || direction.equalsIgnoreCase("RIGHT"))
            buffer[7] = (byte) -magnitude;

        if (direction.equalsIgnoreCase("CCW") || direction.equalsIgnoreCase("LEFT"))
            buffer[7] = magnitude;
        
        return buffer;
    }
}
