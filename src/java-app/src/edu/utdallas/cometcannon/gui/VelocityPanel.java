package edu.utdallas.cometcannon.gui;

import javax.swing.*;
import java.awt.*;

public class VelocityPanel extends JPanel
{
    public VelocityPanel()
    {
        setLayout(new BorderLayout());
        JLabel panelTitle = new JLabel("Velocity", SwingConstants.CENTER);
        VisualVelocityPanel visPanel = new VisualVelocityPanel();
        add(panelTitle, BorderLayout.NORTH);
        add(visPanel, BorderLayout.CENTER);
    }

    class VisualVelocityPanel extends JPanel
    {
        private final int CHASSIS_WIDTH = (int) ((MainFrame.WIDTH / 2) * 0.6);
        private final int CHASSIS_HEIGHT = (int) (MainFrame.HEIGHT * 0.6);
        private final int CHASSIS_NORTH_COORDINATE = (int) (MainFrame.HEIGHT * 0.05);
        private final int CHASSIS_SOUTH_COORDINATE = CHASSIS_NORTH_COORDINATE + CHASSIS_HEIGHT;
        private final int CHASSIS_WEST_COORDINATE = MainFrame.WIDTH / 4 - CHASSIS_WIDTH / 2;
        private final int CHASSIS_EAST_COORDINATE = CHASSIS_WEST_COORDINATE + CHASSIS_WIDTH;
        private final int WHEEL_WIDTH = (int) (CHASSIS_WIDTH * 0.1);
        private final int WHEEL_HEIGHT = (int) (CHASSIS_HEIGHT * 0.25);
        private final int LEFT_WHEEL_WEST_COORDINATE = CHASSIS_WEST_COORDINATE - WHEEL_WIDTH;
        private final int RIGHT_WHEEL_WEST_COORDINATE = CHASSIS_EAST_COORDINATE;
        private final int WHEEL_CHASSIS_VERT_OFFSET = (int) (CHASSIS_HEIGHT * 0.1);
        private final int FRONT_WHEEL_NORTH_COORDINATE = CHASSIS_NORTH_COORDINATE + WHEEL_CHASSIS_VERT_OFFSET;
        private final int REAR_WHEEL_NORTH_COORDINATE = CHASSIS_SOUTH_COORDINATE - WHEEL_CHASSIS_VERT_OFFSET - WHEEL_HEIGHT;

        @Override
        public void paint(Graphics g)
        {
            g.drawRect(CHASSIS_WEST_COORDINATE, CHASSIS_NORTH_COORDINATE, CHASSIS_WIDTH, CHASSIS_HEIGHT);
            g.drawRect(LEFT_WHEEL_WEST_COORDINATE, FRONT_WHEEL_NORTH_COORDINATE, WHEEL_WIDTH, WHEEL_HEIGHT);
            g.drawRect(LEFT_WHEEL_WEST_COORDINATE, REAR_WHEEL_NORTH_COORDINATE, WHEEL_WIDTH, WHEEL_HEIGHT);
            g.drawRect(RIGHT_WHEEL_WEST_COORDINATE, FRONT_WHEEL_NORTH_COORDINATE, WHEEL_WIDTH, WHEEL_HEIGHT);
            g.drawRect(RIGHT_WHEEL_WEST_COORDINATE, REAR_WHEEL_NORTH_COORDINATE, WHEEL_WIDTH, WHEEL_HEIGHT);
        }
    }
}
