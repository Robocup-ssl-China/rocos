

///* this ALWAYS GENERATED file contains the definitions for the interfaces */


// /* File created by MIDL compiler version 6.00.0361 */
///* at Tue Apr 22 16:09:58 2014
// */
///* Compiler settings for \Users\Zjunlict\Desktop\simulator\windows\server.idl:
//    Oicf, W1, Zp8, env=Win32 (32b run)
//    protocol : dce , ms_ext, c_ext, robust
//    error checks: allocation ref bounds_check enum stub_data
//    VC __declspec() decoration level:
//         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
//         DECLSPEC_UUID(), MIDL_INTERFACE()
//*/
////@@MIDL_FILE_HEADING(  )

//#pragma warning( disable: 4049 )  /* more than 64k source lines */


///* verify that the <rpcndr.h> version is high enough to compile this file*/
//#ifndef __REQUIRED_RPCNDR_H_VERSION__
//#define __REQUIRED_RPCNDR_H_VERSION__ 475
//#endif

//#include "rpc.h"
//#include "rpcndr.h"
//#include "staticparams.h"
//#include "geometry.h"

//#ifndef __RPCNDR_H_VERSION__
//#error this stub requires an updated version of <rpcndr.h>
//#endif // __RPCNDR_H_VERSION__

//#ifndef COM_NO_WINDOWS_H
//#define WIN32_LEAN_AND_MEAN
//#include "windows.h"
//#include "ole2.h"
//#include "staticparams.h"
//#endif /*COM_NO_WINDOWS_H*/

//#ifndef __server_h__
//#define __server_h__

//#if defined(_MSC_VER) && (_MSC_VER >= 1020)
//#pragma once
//#endif

///* Forward Declarations */

//#ifndef __IPlayer_FWD_DEFINED__
//#define __IPlayer_FWD_DEFINED__
//typedef interface IPlayer IPlayer;
//#endif 	/* __IPlayer_FWD_DEFINED__ */


//#ifndef __ITeam_FWD_DEFINED__
//#define __ITeam_FWD_DEFINED__
//typedef interface ITeam ITeam;
//#endif 	/* __ITeam_FWD_DEFINED__ */


//#ifndef __IViewer_FWD_DEFINED__
//#define __IViewer_FWD_DEFINED__
//typedef interface IViewer IViewer;
//#endif 	/* __IViewer_FWD_DEFINED__ */


//#ifndef __IController_FWD_DEFINED__
//#define __IController_FWD_DEFINED__
//typedef interface IController IController;
//#endif 	/* __IController_FWD_DEFINED__ */


//#ifndef __IPlayerModule_FWD_DEFINED__
//#define __IPlayerModule_FWD_DEFINED__
//typedef interface IPlayerModule IPlayerModule;
//#endif 	/* __IPlayerModule_FWD_DEFINED__ */


//#ifndef __IPlayer_FWD_DEFINED__
//#define __IPlayer_FWD_DEFINED__
//typedef interface IPlayer IPlayer;
//#endif 	/* __IPlayer_FWD_DEFINED__ */


//#ifndef __ITeam_FWD_DEFINED__
//#define __ITeam_FWD_DEFINED__
//typedef interface ITeam ITeam;
//#endif 	/* __ITeam_FWD_DEFINED__ */


//#ifndef __IViewer_FWD_DEFINED__
//#define __IViewer_FWD_DEFINED__
//typedef interface IViewer IViewer;
//#endif 	/* __IViewer_FWD_DEFINED__ */


//#ifndef __IController_FWD_DEFINED__
//#define __IController_FWD_DEFINED__
//typedef interface IController IController;
//#endif 	/* __IController_FWD_DEFINED__ */


//#ifndef __IPlayerModule_FWD_DEFINED__
//#define __IPlayerModule_FWD_DEFINED__
//typedef interface IPlayerModule IPlayerModule;
//#endif 	/* __IPlayerModule_FWD_DEFINED__ */


//#ifndef __SimServer_FWD_DEFINED__
//#define __SimServer_FWD_DEFINED__

//#ifdef __cplusplus
//typedef class SimServer SimServer;
//#else
//typedef struct SimServer SimServer;
//#endif /* __cplusplus */

//#endif 	/* __SimServer_FWD_DEFINED__ */


//#ifndef __SimPlayerModule_FWD_DEFINED__
//#define __SimPlayerModule_FWD_DEFINED__

//#ifdef __cplusplus
//typedef class SimPlayerModule SimPlayerModule;
//#else
//typedef struct SimPlayerModule SimPlayerModule;
//#endif /* __cplusplus */

//#endif 	/* __SimPlayerModule_FWD_DEFINED__ */


///* header files for imported files */
//#include "unknwn.h"

//#ifdef __cplusplus
//extern "C"{
//#endif

//void * __RPC_USER MIDL_user_allocate(size_t);
//void __RPC_USER MIDL_user_free( void * );

//#ifndef __IPlayer_INTERFACE_DEFINED__
//#define __IPlayer_INTERFACE_DEFINED__

///* interface IPlayer */
///* [oleautomation][uuid][object] */


//EXTERN_C const IID IID_IPlayer;

//#if defined(__cplusplus) && !defined(CINTERFACE)

//    MIDL_INTERFACE("9026A6CF-BF02-486e-A705-354C79490EA3")
//    IPlayer : public IUnknown
//    {
//    public:
//        virtual HRESULT STDMETHODCALLTYPE JoinIn(
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ int type) = 0;

//        virtual HRESULT STDMETHODCALLTYPE Leave( void) = 0;

//        virtual HRESULT STDMETHODCALLTYPE Act(
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3) = 0;

//    };

//#else 	/* C style interface */

//    typedef struct IPlayerVtbl
//    {
//        BEGIN_INTERFACE

//        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
//            IPlayer * This,
//            /* [in] */ REFIID riid,
//            /* [iid_is][out] */ void **ppvObject);

//        ULONG ( STDMETHODCALLTYPE *AddRef )(
//            IPlayer * This);

//        ULONG ( STDMETHODCALLTYPE *Release )(
//            IPlayer * This);

//        HRESULT ( STDMETHODCALLTYPE *JoinIn )(
//            IPlayer * This,
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ int type);

//        HRESULT ( STDMETHODCALLTYPE *Leave )(
//            IPlayer * This);

//        HRESULT ( STDMETHODCALLTYPE *Act )(
//            IPlayer * This,
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3);

//        END_INTERFACE
//    } IPlayerVtbl;

//    interface IPlayer
//    {
//        CONST_VTBL struct IPlayerVtbl *lpVtbl;
//    };



//#ifdef COBJMACROS


//#define IPlayer_QueryInterface(This,riid,ppvObject)	\
//    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

//#define IPlayer_AddRef(This)	\
//    (This)->lpVtbl -> AddRef(This)

//#define IPlayer_Release(This)	\
//    (This)->lpVtbl -> Release(This)


//#define IPlayer_JoinIn(This,side,num,type)	\
//    (This)->lpVtbl -> JoinIn(This,side,num,type)

//#define IPlayer_Leave(This)	\
//    (This)->lpVtbl -> Leave(This)

//#define IPlayer_Act(This,type,value1,value2,value3)	\
//    (This)->lpVtbl -> Act(This,type,value1,value2,value3)

//#endif /* COBJMACROS */


//#endif 	/* C style interface */



//HRESULT STDMETHODCALLTYPE IPlayer_JoinIn_Proxy(
//    IPlayer * This,
//    /* [in] */ int side,
//    /* [in] */ int num,
//    /* [in] */ int type);


//void __RPC_STUB IPlayer_JoinIn_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IPlayer_Leave_Proxy(
//    IPlayer * This);


//void __RPC_STUB IPlayer_Leave_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IPlayer_Act_Proxy(
//    IPlayer * This,
//    /* [in] */ short type,
//    /* [in] */ short value1,
//    /* [in] */ short value2,
//    /* [in] */ short value3);


//void __RPC_STUB IPlayer_Act_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);



//#endif 	/* __IPlayer_INTERFACE_DEFINED__ */


//#ifndef __ITeam_INTERFACE_DEFINED__
//#define __ITeam_INTERFACE_DEFINED__

///* interface ITeam */
///* [oleautomation][uuid][object] */


//EXTERN_C const IID IID_ITeam;

//#if defined(__cplusplus) && !defined(CINTERFACE)

//    MIDL_INTERFACE("729D6232-C5A7-4d5e-95B0-09C486B876FC")
//    ITeam : public IUnknown
//    {
//    public:
//        virtual HRESULT STDMETHODCALLTYPE TeamJoinIn(
//            /* [in] */ int side,
//            /* [in] */ int color,
//            /* [in] */ int type) = 0;

//        virtual HRESULT STDMETHODCALLTYPE TeamLeave( void) = 0;

//        virtual HRESULT STDMETHODCALLTYPE TeamAct(
//            /* [in] */ int num,
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3) = 0;

//    };

//#else 	/* C style interface */

//    typedef struct ITeamVtbl
//    {
//        BEGIN_INTERFACE

//        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
//            ITeam * This,
//            /* [in] */ REFIID riid,
//            /* [iid_is][out] */ void **ppvObject);

//        ULONG ( STDMETHODCALLTYPE *AddRef )(
//            ITeam * This);

//        ULONG ( STDMETHODCALLTYPE *Release )(
//            ITeam * This);

//        HRESULT ( STDMETHODCALLTYPE *TeamJoinIn )(
//            ITeam * This,
//            /* [in] */ int side,
//            /* [in] */ int color,
//            /* [in] */ int type);

//        HRESULT ( STDMETHODCALLTYPE *TeamLeave )(
//            ITeam * This);

//        HRESULT ( STDMETHODCALLTYPE *TeamAct )(
//            ITeam * This,
//            /* [in] */ int num,
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3);

//        END_INTERFACE
//    } ITeamVtbl;

//    interface ITeam
//    {
//        CONST_VTBL struct ITeamVtbl *lpVtbl;
//    };



//#ifdef COBJMACROS


//#define ITeam_QueryInterface(This,riid,ppvObject)	\
//    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

//#define ITeam_AddRef(This)	\
//    (This)->lpVtbl -> AddRef(This)

//#define ITeam_Release(This)	\
//    (This)->lpVtbl -> Release(This)


//#define ITeam_TeamJoinIn(This,side,color,type)	\
//    (This)->lpVtbl -> TeamJoinIn(This,side,color,type)

//#define ITeam_TeamLeave(This)	\
//    (This)->lpVtbl -> TeamLeave(This)

//#define ITeam_TeamAct(This,num,type,value1,value2,value3)	\
//    (This)->lpVtbl -> TeamAct(This,num,type,value1,value2,value3)

//#endif /* COBJMACROS */


//#endif 	/* C style interface */



//HRESULT STDMETHODCALLTYPE ITeam_TeamJoinIn_Proxy(
//    ITeam * This,
//    /* [in] */ int side,
//    /* [in] */ int color,
//    /* [in] */ int type);


//void __RPC_STUB ITeam_TeamJoinIn_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE ITeam_TeamLeave_Proxy(
//    ITeam * This);


//void __RPC_STUB ITeam_TeamLeave_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE ITeam_TeamAct_Proxy(
//    ITeam * This,
//    /* [in] */ int num,
//    /* [in] */ short type,
//    /* [in] */ short value1,
//    /* [in] */ short value2,
//    /* [in] */ short value3);


//void __RPC_STUB ITeam_TeamAct_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);



//#endif 	/* __ITeam_INTERFACE_DEFINED__ */


///* interface __MIDL_itf_server_0010 */
///* [local] */

#ifndef __ZSS_SERVER_H__
#define __ZSS_SERVER_H__
#include "staticparams.h"
#include "geometry.h"
#include "staticparams.h"
struct PosT {
    PosT(): valid(0), x(0), y(0) {}
    int valid;
    float x;
    float y;
    void setValid(int _value) {
        valid = _value;
    }
};

struct VehicleInfoT {
    VehicleInfoT(): dir(0), type(0), dirvel(0) {}
    PosT pos;
    PosT rawPos;
    CVector vel;
    CVector raw_vel;
    float dir;//radius
    float rawdir;
    float dirvel;
    float raw_dirVel;
    CVector accelerate;
    int type;
};
enum ballState {received, touched, kicked, struggle, chip_pass, flat_pass};

struct VisualInfoT {
    VisualInfoT(): cycle(0), BallState(received), BallLastTouch(0), mode(0) {}
    unsigned int cycle;
    VehicleInfoT player[PARAM::TEAMS][PARAM::Field::MAX_PLAYER];
    PosT ball;
    PosT rawBall;
    PosT chipPredict;
    CVector BallVel;
    ballState BallState;
    int BallLastTouch;
    int mode;
    int next_command;
    //unsigned char ourRobotIndexBefore[ PARAM::Field::MAX_PLAYER] = {};
    int before_cycle[ 2 * PARAM::Field::MAX_PLAYER] = {};
    // unsigned char theirRobotIndexBefore[ PARAM::Field::MAX_PLAYER] = {};
    //PosT imageBall;
    PosT ballPlacePosition;
};
#endif // __ZSS_SERVER_H__


//extern RPC_IF_HANDLE __MIDL_itf_server_0010_v0_0_c_ifspec;
//extern RPC_IF_HANDLE __MIDL_itf_server_0010_v0_0_s_ifspec;

//#ifndef __IViewer_INTERFACE_DEFINED__
//#define __IViewer_INTERFACE_DEFINED__

///* interface IViewer */
///* [oleautomation][uuid][object] */


//EXTERN_C const IID IID_IViewer;

//#if defined(__cplusplus) && !defined(CINTERFACE)

//    MIDL_INTERFACE("630015B6-EBAF-43c0-9D4E-093057F0976A")
//    IViewer : public IUnknown
//    {
//    public:
//        virtual HRESULT STDMETHODCALLTYPE GetVisualInfo(
//            /* [retval][out] */ VisualInfoT *pinfo) = 0;

//        virtual HRESULT STDMETHODCALLTYPE GetRawVisualInfo(
//            /* [retval][out] */ VisualInfoT *pinfo) = 0;

//    };

//#else 	/* C style interface */

//    typedef struct IViewerVtbl
//    {
//        BEGIN_INTERFACE

//        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
//            IViewer * This,
//            /* [in] */ REFIID riid,
//            /* [iid_is][out] */ void **ppvObject);

//        ULONG ( STDMETHODCALLTYPE *AddRef )(
//            IViewer * This);

//        ULONG ( STDMETHODCALLTYPE *Release )(
//            IViewer * This);

//        HRESULT ( STDMETHODCALLTYPE *GetVisualInfo )(
//            IViewer * This,
//            /* [retval][out] */ VisualInfoT *pinfo);

//        HRESULT ( STDMETHODCALLTYPE *GetRawVisualInfo )(
//            IViewer * This,
//            /* [retval][out] */ VisualInfoT *pinfo);

//        END_INTERFACE
//    } IViewerVtbl;

//    interface IViewer
//    {
//        CONST_VTBL struct IViewerVtbl *lpVtbl;
//    };



//#ifdef COBJMACROS


//#define IViewer_QueryInterface(This,riid,ppvObject)	\
//    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

//#define IViewer_AddRef(This)	\
//    (This)->lpVtbl -> AddRef(This)

//#define IViewer_Release(This)	\
//    (This)->lpVtbl -> Release(This)


//#define IViewer_GetVisualInfo(This,pinfo)	\
//    (This)->lpVtbl -> GetVisualInfo(This,pinfo)

//#define IViewer_GetRawVisualInfo(This,pinfo)	\
//    (This)->lpVtbl -> GetRawVisualInfo(This,pinfo)

//#endif /* COBJMACROS */


//#endif 	/* C style interface */



//HRESULT STDMETHODCALLTYPE IViewer_GetVisualInfo_Proxy(
//    IViewer * This,
//    /* [retval][out] */ VisualInfoT *pinfo);


//void __RPC_STUB IViewer_GetVisualInfo_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IViewer_GetRawVisualInfo_Proxy(
//    IViewer * This,
//    /* [retval][out] */ VisualInfoT *pinfo);


//void __RPC_STUB IViewer_GetRawVisualInfo_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);



//#endif 	/* __IViewer_INTERFACE_DEFINED__ */


//#ifndef __IController_INTERFACE_DEFINED__
//#define __IController_INTERFACE_DEFINED__

///* interface IController */
///* [oleautomation][uuid][object] */


//EXTERN_C const IID IID_IController;

//#if defined(__cplusplus) && !defined(CINTERFACE)

//    MIDL_INTERFACE("CA4D680D-35F8-4ae5-8A2A-384CF918EF1B")
//    IController : public IUnknown
//    {
//    public:
//        virtual HRESULT STDMETHODCALLTYPE ChangeCollisionMode(
//            /* [in] */ int cMode) = 0;

//        virtual HRESULT STDMETHODCALLTYPE ChangePlaymode(
//            /* [in] */ int pmode) = 0;

//        virtual HRESULT STDMETHODCALLTYPE MovePlayer(
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ double x,
//            /* [in] */ double y,
//            /* [in] */ double angle) = 0;

//        virtual HRESULT STDMETHODCALLTYPE MoveBall(
//            /* [in] */ double x,
//            /* [in] */ double y,
//            /* [in] */ double dx,
//            /* [in] */ double dy) = 0;

//        virtual HRESULT STDMETHODCALLTYPE Pause(
//            /* [in] */ int on) = 0;

//        virtual HRESULT STDMETHODCALLTYPE AddPlayer(
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ int type) = 0;

//        virtual HRESULT STDMETHODCALLTYPE RemovePlayer(
//            /* [in] */ int side,
//            /* [in] */ int num) = 0;

//        virtual HRESULT STDMETHODCALLTYPE PlayerAct(
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3) = 0;

//        virtual HRESULT STDMETHODCALLTYPE SetVisionInfo(
//            /* [in] */ VisualInfoT info) = 0;

//    };

//#else 	/* C style interface */

//    typedef struct IControllerVtbl
//    {
//        BEGIN_INTERFACE

//        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
//            IController * This,
//            /* [in] */ REFIID riid,
//            /* [iid_is][out] */ void **ppvObject);

//        ULONG ( STDMETHODCALLTYPE *AddRef )(
//            IController * This);

//        ULONG ( STDMETHODCALLTYPE *Release )(
//            IController * This);

//        HRESULT ( STDMETHODCALLTYPE *ChangeCollisionMode )(
//            IController * This,
//            /* [in] */ int cMode);

//        HRESULT ( STDMETHODCALLTYPE *ChangePlaymode )(
//            IController * This,
//            /* [in] */ int pmode);

//        HRESULT ( STDMETHODCALLTYPE *MovePlayer )(
//            IController * This,
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ double x,
//            /* [in] */ double y,
//            /* [in] */ double angle);

//        HRESULT ( STDMETHODCALLTYPE *MoveBall )(
//            IController * This,
//            /* [in] */ double x,
//            /* [in] */ double y,
//            /* [in] */ double dx,
//            /* [in] */ double dy);

//        HRESULT ( STDMETHODCALLTYPE *Pause )(
//            IController * This,
//            /* [in] */ int on);

//        HRESULT ( STDMETHODCALLTYPE *AddPlayer )(
//            IController * This,
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ int type);

//        HRESULT ( STDMETHODCALLTYPE *RemovePlayer )(
//            IController * This,
//            /* [in] */ int side,
//            /* [in] */ int num);

//        HRESULT ( STDMETHODCALLTYPE *PlayerAct )(
//            IController * This,
//            /* [in] */ int side,
//            /* [in] */ int num,
//            /* [in] */ short type,
//            /* [in] */ short value1,
//            /* [in] */ short value2,
//            /* [in] */ short value3);

//        HRESULT ( STDMETHODCALLTYPE *SetVisionInfo )(
//            IController * This,
//            /* [in] */ VisualInfoT info);

//        END_INTERFACE
//    } IControllerVtbl;

//    interface IController
//    {
//        CONST_VTBL struct IControllerVtbl *lpVtbl;
//    };



//#ifdef COBJMACROS


//#define IController_QueryInterface(This,riid,ppvObject)	\
//    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

//#define IController_AddRef(This)	\
//    (This)->lpVtbl -> AddRef(This)

//#define IController_Release(This)	\
//    (This)->lpVtbl -> Release(This)


//#define IController_ChangeCollisionMode(This,cMode)	\
//    (This)->lpVtbl -> ChangeCollisionMode(This,cMode)

//#define IController_ChangePlaymode(This,pmode)	\
//    (This)->lpVtbl -> ChangePlaymode(This,pmode)

//#define IController_MovePlayer(This,side,num,x,y,angle)	\
//    (This)->lpVtbl -> MovePlayer(This,side,num,x,y,angle)

//#define IController_MoveBall(This,x,y,dx,dy)	\
//    (This)->lpVtbl -> MoveBall(This,x,y,dx,dy)

//#define IController_Pause(This,on)	\
//    (This)->lpVtbl -> Pause(This,on)

//#define IController_AddPlayer(This,side,num,type)	\
//    (This)->lpVtbl -> AddPlayer(This,side,num,type)

//#define IController_RemovePlayer(This,side,num)	\
//    (This)->lpVtbl -> RemovePlayer(This,side,num)

//#define IController_PlayerAct(This,side,num,type,value1,value2,value3)	\
//    (This)->lpVtbl -> PlayerAct(This,side,num,type,value1,value2,value3)

//#define IController_SetVisionInfo(This,info)	\
//    (This)->lpVtbl -> SetVisionInfo(This,info)

//#endif /* COBJMACROS */


//#endif 	/* C style interface */



//HRESULT STDMETHODCALLTYPE IController_ChangeCollisionMode_Proxy(
//    IController * This,
//    /* [in] */ int cMode);


//void __RPC_STUB IController_ChangeCollisionMode_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_ChangePlaymode_Proxy(
//    IController * This,
//    /* [in] */ int pmode);


//void __RPC_STUB IController_ChangePlaymode_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_MovePlayer_Proxy(
//    IController * This,
//    /* [in] */ int side,
//    /* [in] */ int num,
//    /* [in] */ double x,
//    /* [in] */ double y,
//    /* [in] */ double angle);


//void __RPC_STUB IController_MovePlayer_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_MoveBall_Proxy(
//    IController * This,
//    /* [in] */ double x,
//    /* [in] */ double y,
//    /* [in] */ double dx,
//    /* [in] */ double dy);


//void __RPC_STUB IController_MoveBall_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_Pause_Proxy(
//    IController * This,
//    /* [in] */ int on);


//void __RPC_STUB IController_Pause_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_AddPlayer_Proxy(
//    IController * This,
//    /* [in] */ int side,
//    /* [in] */ int num,
//    /* [in] */ int type);


//void __RPC_STUB IController_AddPlayer_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_RemovePlayer_Proxy(
//    IController * This,
//    /* [in] */ int side,
//    /* [in] */ int num);


//void __RPC_STUB IController_RemovePlayer_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_PlayerAct_Proxy(
//    IController * This,
//    /* [in] */ int side,
//    /* [in] */ int num,
//    /* [in] */ short type,
//    /* [in] */ short value1,
//    /* [in] */ short value2,
//    /* [in] */ short value3);


//void __RPC_STUB IController_PlayerAct_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IController_SetVisionInfo_Proxy(
//    IController * This,
//    /* [in] */ VisualInfoT info);


//void __RPC_STUB IController_SetVisionInfo_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);



//#endif 	/* __IController_INTERFACE_DEFINED__ */


//#ifndef __IPlayerModule_INTERFACE_DEFINED__
//#define __IPlayerModule_INTERFACE_DEFINED__

///* interface IPlayerModule */
///* [oleautomation][uuid][object] */


//EXTERN_C const IID IID_IPlayerModule;

//#if defined(__cplusplus) && !defined(CINTERFACE)

//    MIDL_INTERFACE("9977F039-3816-443f-85E0-4E6E6B1F9070")
//    IPlayerModule : public IUnknown
//    {
//    public:
//        virtual HRESULT STDMETHODCALLTYPE RunVisual(
//            /* [in] */ IUnknown *pViewer) = 0;

//        virtual HRESULT STDMETHODCALLTYPE RunDecision(
//            /* [in] */ IUnknown *pPlayer) = 0;

//    };

//#else 	/* C style interface */

//    typedef struct IPlayerModuleVtbl
//    {
//        BEGIN_INTERFACE

//        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
//            IPlayerModule * This,
//            /* [in] */ REFIID riid,
//            /* [iid_is][out] */ void **ppvObject);

//        ULONG ( STDMETHODCALLTYPE *AddRef )(
//            IPlayerModule * This);

//        ULONG ( STDMETHODCALLTYPE *Release )(
//            IPlayerModule * This);

//        HRESULT ( STDMETHODCALLTYPE *RunVisual )(
//            IPlayerModule * This,
//            /* [in] */ IUnknown *pViewer);

//        HRESULT ( STDMETHODCALLTYPE *RunDecision )(
//            IPlayerModule * This,
//            /* [in] */ IUnknown *pPlayer);

//        END_INTERFACE
//    } IPlayerModuleVtbl;

//    interface IPlayerModule
//    {
//        CONST_VTBL struct IPlayerModuleVtbl *lpVtbl;
//    };



//#ifdef COBJMACROS


//#define IPlayerModule_QueryInterface(This,riid,ppvObject)	\
//    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

//#define IPlayerModule_AddRef(This)	\
//    (This)->lpVtbl -> AddRef(This)

//#define IPlayerModule_Release(This)	\
//    (This)->lpVtbl -> Release(This)


//#define IPlayerModule_RunVisual(This,pViewer)	\
//    (This)->lpVtbl -> RunVisual(This,pViewer)

//#define IPlayerModule_RunDecision(This,pPlayer)	\
//    (This)->lpVtbl -> RunDecision(This,pPlayer)

//#endif /* COBJMACROS */


//#endif 	/* C style interface */



//HRESULT STDMETHODCALLTYPE IPlayerModule_RunVisual_Proxy(
//    IPlayerModule * This,
//    /* [in] */ IUnknown *pViewer);


//void __RPC_STUB IPlayerModule_RunVisual_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);


//HRESULT STDMETHODCALLTYPE IPlayerModule_RunDecision_Proxy(
//    IPlayerModule * This,
//    /* [in] */ IUnknown *pPlayer);


//void __RPC_STUB IPlayerModule_RunDecision_Stub(
//    IRpcStubBuffer *This,
//    IRpcChannelBuffer *_pRpcChannelBuffer,
//    PRPC_MESSAGE _pRpcMessage,
//    DWORD *_pdwStubPhase);



//#endif 	/* __IPlayerModule_INTERFACE_DEFINED__ */



//#ifndef __SmallSim_LIBRARY_DEFINED__
//#define __SmallSim_LIBRARY_DEFINED__

///* library SmallSim */
///* [version][helpstring][uuid] */







//EXTERN_C const IID LIBID_SmallSim;

//EXTERN_C const CLSID CLSID_SimServer;

//#ifdef __cplusplus

//class DECLSPEC_UUID("B24B1E3D-F465-47f0-8072-89648010677F")
//SimServer;
//#endif

//EXTERN_C const CLSID CLSID_SimPlayerModule;

//#ifdef __cplusplus

//class DECLSPEC_UUID("40211A04-89C3-44b0-804B-336B20E94B3F")
//SimPlayerModule;
//#endif
//#endif /* __SmallSim_LIBRARY_DEFINED__ */

///* Additional Prototypes for ALL interfaces */

///* end of Additional Prototypes */

//#ifdef __cplusplus
//}
//#endif

//#endif


