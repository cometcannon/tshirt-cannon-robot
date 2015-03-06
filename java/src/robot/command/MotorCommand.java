package robot.command;

public class MotorCommand implements Command
{
    private byte motorIndex;
    private byte magnitude;
    
    public MotorCommand(int motorIndex, int magnitude)
    {
        this.motorIndex = (byte) motorIndex;
        this.magnitude = (byte) magnitude;
    }
    
    @Override
    public byte[] generatePayload()
    {
        byte buffer[] = new byte[7];
        
        //////////////////////////////////////// 
        // magic bytes
        buffer[0] = 0x47;
        buffer[1] = 0x41;
        buffer[2] = 0x4e;
        buffer[3] = 0x53;
        
        //////////////////////////////////////// 
        // command type
        buffer[4] = 4; 
 
        //////////////////////////////////////// 
        // motor index
        buffer[5] = motorIndex;

        //////////////////////////////////////// 
        // magnitude
        buffer[6] = magnitude;
        
        return buffer;
    }
}
