package edu.utdallas.cometcannon.robot.command;

public class MoveDirectionCommand implements Command
{
    private String direction;
    private byte magnitude;

    public MoveDirectionCommand(String direction, int magnitude)
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
        buffer[2] = 0x4e;
        buffer[3] = 0x53;
        
        //////////////////////////////////////// 
        // command type
        buffer[4] = 3; 
        
        //////////////////////////////////////// 
        // velocity vector
        buffer[5] = 0; // v_x
        buffer[6] = 0; // v_y
        buffer[7] = 0; // w_z
        
        if (direction.equalsIgnoreCase("FORWARD"))
            buffer[5] = magnitude;
        
        if (direction.equalsIgnoreCase("BACKWARD"))
            buffer[5] = (byte) -magnitude;
        
        if (direction.equalsIgnoreCase("LEFT"))
            buffer[6] = magnitude;

        if (direction.equalsIgnoreCase("RIGHT"))
            buffer[7] = (byte) -magnitude;
        
        return buffer;
    }
}
