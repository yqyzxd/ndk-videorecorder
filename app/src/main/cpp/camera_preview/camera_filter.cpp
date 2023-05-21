//
// Created by 史浩 on 2023/5/6.
//1）C++构造器可以为虚拟函数吗？
//
//不行。因为构造器调用的时候会设置当前类的虚表地址，然后如果构造函数本身为虚拟函数，那么调用构造函数就需要提前访问虚表，此时虚表指针还没有指向虚表，就没法访问虚表。
//
//（2）构造函数中可以调用虚拟函数吗？
//
//可以，但是调用虚拟函数就是调用跟调用普通函数效果一样。
//
//因为子类在创建的时候是先调用父类的构造器，父类的构造器先设置好父类的虚表地址，然后再执行构造器中的代码，然后在回到子类构造器，先将虚表地址替换为子类的虚表地址，然后执行子类构造器的代码。
//
//因此如果在父类构造器中调用虚函数，那么此时是从父类的虚表中查找函数地址，查找到的还是父类的虚函数地址。因此会调用父类的虚函数，而不会调用子类的虚函数。
//
//（3）析构函数中可以调用虚拟函数吗？
//
//可以。但是调用虚拟函数跟调用普通函数没啥区别。
//
//因为析构的时候先执行子类的析构函数，再执行父类的析构函数，在执行当前类的析构函数的时候会先将虚表地址设置为当前类的虚表地址，然后再从虚表里查找函数地址，因此找到的只能是当前的虚函数的地址。
//
//（4）父类的析构函数为什么要为虚函数？
//
//因为如果父类析构函数不为虚函数，那么当对象析构的时候只会调用指针当前类型的析构函数，不会调用父类或者子类的析构函数，会导致资源泄漏。
//
//如果父类的析构函数为虚函数，那么子类的析构函数在析构的时候会先调用子类的析构函数，再调用父类的析构函数。这样子就不会存在资源泄漏。
//
//注：
//
//（1）将子类和父类构造器和析构器调用串起来是编译器实现的
//
//（2）子类和父类都有自己的虚表。子类或者父类的构造器和析构器在最开始都是将虚表设置为当前类的虚表。然后才真正调用构造函数和析构函数里的代码。
//————————————————
//版权声明：本文为CSDN博主「lk_HIT」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/lk_HIT/article/details/122526416
//
#include "camera_filter.h"
#include "gles/gl_utils.h"

#define LOG_TAG "CameraFilter"
CameraFilter::CameraFilter()
        : BaseFboFilter(camera_vertex, camera_fragment) {

    //camera渲染器额外的location，不能定义在getLocation中，因为在父类构造函数中只能调用父类自己的getLocation方法，是调用不到子类覆写的getLocation方法的
    uMatrixLocation= glGetUniformLocation(mProgram,"u_Matrix");
    LOGI("uMatrixLocation:%d",uMatrixLocation);
}
CameraFilter::~CameraFilter() noexcept {

}

GLuint CameraFilter::getTextureTarget() {
    return GL_TEXTURE_EXTERNAL_OES;
}



void CameraFilter::inflateLocation(GLuint textureId) {
    BaseFboFilter::inflateLocation(textureId);
    if (matrix== nullptr){
        //单位矩阵
        matrix=identity_matrix;
    }
    glUniformMatrix4fv(uMatrixLocation,1, GL_FALSE,matrix);
    checkGlError("glUniformMatrix4fv uMatrixLocation");
}
//设置相机传递过来的旋转矩阵
void CameraFilter::setMatrix(GLfloat *matrix) {
    this->matrix=matrix;
}
