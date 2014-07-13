#define __BlockUtility_H_INCLUDED__   //   #define this so the compiler knows it has been included
class BlockUtility
{
   public:
        BlockUtility();
        void conjugate(unsigned char *);
        void setComplexityThreshold(float);
        float getComplexityThreshold();
        float getComplexity(unsigned char *);
        int isComplex(unsigned char *);
        void printBitPlane(unsigned char *);
    private:
        float threshold;
        int x;
        int y;
};