using System;
using System.Diagnostics;

namespace WPFDX11Sample
{
    public class RenderFrameTimer
    {
        private readonly Stopwatch stopwatch;
        private readonly long frameFrequency;
        private long callCount;
        private long frameCount;
        private long accumulatedFrameCount;
        private long accumulatedTime;
        private long ticks0, ticks1;
        public RenderFrameTimer(long frameFrequency)
        {
            stopwatch = new Stopwatch();
            this.frameFrequency = frameFrequency > 0 ? frameFrequency : 1;
            Reset();
        }

        public void Reset()
        {
            stopwatch.Reset();
            callCount = frameFrequency;
            frameCount = 0;
            accumulatedFrameCount = 0;
            accumulatedTime = 0;
            ticks0 = stopwatch.ElapsedTicks;
            ticks1 = ticks0;
        }
        public void Measure()
        {
            if (--callCount == 0)
            {
                stopwatch.Stop();
                ticks1 = stopwatch.ElapsedTicks;
                long delta = (ticks1 - ticks0) * 1000L / Stopwatch.Frequency;
                ticks0 = ticks1;
                accumulatedFrameCount += frameCount;
                accumulatedTime += delta;
                frameCount = 0;
                callCount = frameFrequency;
                stopwatch.Start();
            }
        }

        public double GetFramesPerSecond()
        {
            if (accumulatedTime > 0)
            {
                double numFrames = (double)accumulatedFrameCount;
                double numMilliseconds = (double)accumulatedTime;
                return 1000.0 * numFrames / numMilliseconds;
            }
            else
            {
                return 0.0;
            }
        }
        public void UpdateFrameCount()
        {
            ++frameCount;
        }
    }
}
