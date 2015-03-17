package edu.utdallas.cometcannon.gui;

import javax.swing.JSlider;

public class MagnitudeSlider extends JSlider
{
    protected MagnitudeSlider()
    {
        super(JSlider.VERTICAL, 0, 127, 0);
        
        setMajorTickSpacing(20);
        setMinorTickSpacing(5);
        setPaintTicks(true);
        setPaintLabels(true);
    }
}
