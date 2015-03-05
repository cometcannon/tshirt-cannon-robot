package robot.command;

public class SetPressureCommand implements Command
{
    private byte pressure;

    public SetPressureCommand(int pressure)
    {
        this.pressure = (byte) pressure;
    }
    
    @Override
    public byte[] generatePayload()
    {
        byte buffer[] = new byte[6];

        //////////////////////////////////////// 
        // magic bytes
        buffer[0] = 0x47;
        buffer[1] = 0x41;
        buffer[2] = 0x4e;
        buffer[3] = 0x53;
        
        //////////////////////////////////////// 
        // command type
        buffer[4] = 6; 
        
        //////////////////////////////////////// 
        // pressure value
        buffer[5] = pressure;

        return buffer;
    }
}
