package edu.utdallas.cometcannon.robot.command;

public class SendAngularVelocityCommand implements Command
{
    public SendAngularVelocityCommand()
    {

    }

    @Override
    public byte[] generatePayload()
    {
        byte buffer[] = new byte[5];

        ////////////////////////////////////////
        // magic bytes
        buffer[0] = 0x47;
        buffer[1] = 0x41;
        buffer[2] = 0x4e;
        buffer[3] = 0x53;

        ////////////////////////////////////////
        // command type
        buffer[4] = 4;

        return buffer;
    }
}
