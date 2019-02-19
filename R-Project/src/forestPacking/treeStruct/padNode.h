#ifndef padNode_h
#define padNode_h
#include <cstdint>

class alignas(32) padNode 
{


    public:
        uint32_t left;
        uint32_t feature;
        double cutValue;
        uint32_t right;

        inline bool isInternalNode(){
            return left;
        }

        void setNode(double cVal, uint32_t feat, uint32_t l, uint32_t r);
        void virtual printNode();
        double returnCutValue();
        inline int returnFeature(){
            return feature;
        }
        int returnLeftNode(); //should have been Value instead of Node

        //should have been Value instead of Node
        inline int returnRightNode(){
            return right;
        }
        int returnClass();//should have been Value instead of Node
        void setClass(int classNum);
        void setLeftValue(uint32_t LVal);
        void setRightValue(uint32_t RVal); 
        bool goLeft(double featureValue);
        inline int nextNode(double featureValue){
            return (featureValue < cutValue) ? left : right;
        }

};
#endif //padNode_h
