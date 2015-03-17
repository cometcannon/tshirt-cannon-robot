package edu.utdallas.cometcannon.gui;

import javax.swing.JComboBox;

public class CommandComboBox extends JComboBox
{
    final String[] commands = {"FORWARD", "BACKWARD", "LEFT", "RIGHT", "ROTATE CW",
                               "ROTATE CCW", "MOTOR 0", "MOTOR 1", "MOTOR 2", "MOTOR 3", "FIRE", "PRESSURE"};

    protected CommandComboBox()
    {
        for (int i = 0; i < commands.length; i++)
            addItem(commands[i]);
    }
}
