/***********************************************\
Architecture layout prototype
\***********************************************/
#if 0

//----------------------Rendering----------------------------
class CMesh
{
public:
    void Draw( void )
    {
        SetBuffersAndStuff();
        DrawPrimitive();
    }

private:
    VertexBuffer*       pVertexBuffer;
    IndexBuffer*        pIndexBuffer;
    VertexFormatStuff   pVertexFormat;
};

class CMaterial
{
public:
    void SetMaterial( void )
    {
        SetVShader();
        SetPShader();
        SetTexture();
    }

private:
    VertexShader*   pVertexShader;
    PixelShader*    pPixelShader;
    Texture*        pTexture;
};
//-----------------------------------------------------------

class CPhysicsObject
{
public:

private:    
    Position        vPosition;
    Quaternion      Orientation;
    BoundingVolume* pBoundingVolume;
    Velocity        vVelocity;
    float           fRotation;
    float           fSpin;
};


//-------------------------Object----------------------------
class CObject
{
public:


private:
    CMaterial*      pMaterial;
    CMesh*          pMesh;

    Position        vPosition;
    Quaternion      Orientation;
    BoundingVolume* pBoundingVolume;
    CPhysicsObject* pPhysics;
};

#endif
