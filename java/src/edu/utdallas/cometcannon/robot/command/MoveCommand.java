package edu.utdallas.cometcannon.robot.command;

public class MoveCommand implements Command
{
    private byte v_x;
    private byte v_y;
    private byte w_z;

    public MoveCommand(byte v_x, byte v_y, byte w_z)
    {
        this.v_x = (byte) v_x;
        this.v_y = (byte) v_y;
        this.w_z = (byte) w_z;
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
        buffer[5] = this.v_x;
        buffer[6] = this.v_y;
        buffer[7] = this.w_z;

        return buffer;
    }
}
