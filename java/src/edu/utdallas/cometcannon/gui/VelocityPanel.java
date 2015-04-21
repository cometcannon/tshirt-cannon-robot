package edu.utdallas.cometcannon.gui;

import java.util.concurrent.*;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;

import edu.utdallas.cometcannon.robot.command.*;

public class VelocityPanel extends JPanel
{
    private ArrayBlockingQueue<Command> robotCommandQueue;
    private VisualVelocityPanel visPanel;
    private JLabel panelTitle;
    private int v_x;
    private int v_y;

    public VelocityPanel(ArrayBlockingQueue<Command> robotCommandQueue)
    {
        this.robotCommandQueue = robotCommandQueue;

        setLayout(new BorderLayout());
        visPanel = new VisualVelocityPanel();
        panelTitle = new JLabel("Velocity", SwingConstants.CENTER);
        add(panelTitle, BorderLayout.NORTH);
        add(visPanel, BorderLayout.CENTER);

        Thread t = new Thread(new VelocityUpdater());
        t.start();
    }

    public void updateRobotSize()
    {
        visPanel.repaint();
    }

    class VelocityUpdater implements Runnable
    {
        @Override
        public void run()
        {
            while (true) {

                v_x = MainFrame.V_X;
                v_y = MainFrame.V_Y;
                visPanel.repaint();

                try { Thread.sleep(10); } catch (InterruptedException ex) { }
            }
        }
    }

    class VisualVelocityPanel extends JPanel
    {
        void drawArrow(Graphics g, int x1, int y1, int x2, int y2)
        {
            final int HEAD_SIZE = 4;

            Graphics2D g2d = (Graphics2D) g.create();

            double dx = x2 - x1;
            double dy = y2 - y1;

            double angle = Math.atan2(dy, dx);
            int len = (int) Math.sqrt(dx*dx + dy*dy);

            AffineTransform at = AffineTransform.getTranslateInstance(x1, y1);
            at.concatenate(AffineTransform.getRotateInstance(angle));
            g2d.transform(at);

            // Draw horizontal arrow starting in (0, 0)
            g2d.drawLine(0, 0, len, 0);
            g2d.fillPolygon(new int[] {len + 5, len-HEAD_SIZE, len-HEAD_SIZE, len},
                            new int[] {0, -HEAD_SIZE, HEAD_SIZE, 0}, 4);
        }

        @Override
        public void paint(Graphics g)
        {
            super.paintComponent(g);

            Graphics2D g2d = (Graphics2D) g;

            int chassisWidth = (int) ((MainFrame.WIDTH / 2) * 0.6);
            int chassisHeight = (int) (MainFrame.HEIGHT * 0.55);
            int chassisNorthCoordinate = (int) (MainFrame.HEIGHT * 0.05);
            int chassisSouthCoordinate = chassisNorthCoordinate + chassisHeight;
            int chassisWestCoordinate = MainFrame.WIDTH / 4 - chassisWidth / 2;
            int chassisEastCoordinate = chassisWestCoordinate + chassisWidth;
            int wheelWidth = (int) (chassisWidth * 0.1);
            int wheelHeight = (int) (chassisHeight * 0.25);
            int leftWheelWestCoordinate = chassisWestCoordinate - wheelWidth;
            int rightWheelWestCoordinate = chassisEastCoordinate;
            int wheelChassisVertOffset = (int) (chassisHeight * 0.1);
            int frontWheelNorthCoordinate = chassisNorthCoordinate + wheelChassisVertOffset;
            int rearWheelNorthCoordinate = chassisSouthCoordinate - wheelChassisVertOffset - wheelHeight;
            int chassisHorizontalMidline = chassisWestCoordinate + (int) (0.5 * chassisWidth);
            int chassisVerticalMidline = chassisNorthCoordinate + (int) (0.5 * chassisHeight);

            g2d.setStroke(new BasicStroke(2));

            if (Math.abs(v_x) > 10 || Math.abs(v_y) > 10) {
                drawArrow(g2d,
                          chassisHorizontalMidline,
                          chassisVerticalMidline,
                          chassisHorizontalMidline - v_y * (int) (chassisWidth * 0.4) / 128,
                          chassisVerticalMidline - v_x * (int) (chassisHeight * 0.4) / 128);
            }

            g2d.drawRect(chassisWestCoordinate,
                       chassisNorthCoordinate,
                       chassisWidth,
                       chassisHeight);

            g2d.drawRect(leftWheelWestCoordinate,
                       frontWheelNorthCoordinate,
                       wheelWidth,
                       wheelHeight);

            g2d.drawRect(leftWheelWestCoordinate,
                       rearWheelNorthCoordinate,
                       wheelWidth,
                       wheelHeight);

            g2d.drawRect(rightWheelWestCoordinate,
                       frontWheelNorthCoordinate,
                       wheelWidth,
                       wheelHeight);

            g2d.drawRect(rightWheelWestCoordinate,
                       rearWheelNorthCoordinate,
                       wheelWidth,
                       wheelHeight);
        }
    }
}
