//=============================================================================
//		３Ｄゲームプログラム
//		Ｃ３ＤＡｃｔ１１１　　　　　　                 ver 3.2        2023.1.31
//
//		ビルボード炎の処理
//															EffectBillFire.cpp
//=============================================================================
#include  "EffectBillFire.h"

//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャクラスのコンストラクタ	
//
//   なお、プロシージャのdeleteはCBaseProcのデストラクタで行うため不要
//
//------------------------------------------------------------------------
CEffectBillFireProc::CEffectBillFireProc()
{
	BILLBOARDBASE bb = {};

	// ------------------------------------------------------------------------------------------
	// ビルボード構造体配列の設定

	// 0:炎ビルボード
	m_BillboardArray.push_back(bb);
	Load(_T("Data/Image/sparklen3.png"), &m_BillboardArray.back());
	m_BillboardArray.back().m_fDestWidth = 0.7f;      // 表示幅
	m_BillboardArray.back().m_fDestHeight = 0.7f;     // 表示高さ
	m_BillboardArray.back().m_fDestCenterX = m_BillboardArray.back().m_fDestWidth / 2;  // 表示中心位置Ｘ(真ん中)
	m_BillboardArray.back().m_fDestCenterY = m_BillboardArray.back().m_fDestHeight / 2; // 表示中心位置Ｙ(真ん中)
	m_BillboardArray.back().m_dwSrcX = 0;             // パターンの位置　Ｘ座標
	m_BillboardArray.back().m_dwSrcY = 0;             // パターンの位置　Ｙ座標
	m_BillboardArray.back().m_dwSrcWidth = 85;        // パターンの幅
	m_BillboardArray.back().m_dwSrcHeight = 85;       // パターンの高さ
	m_BillboardArray.back().m_dwNumX = 3;             // アニメーションさせるパターンの数　Ｘ方向
	m_BillboardArray.back().m_dwNumY = 1;             // アニメーションさせるパターンの数　Ｙ方向
	m_BillboardArray.back().m_fAlpha = 0.9f;          // 透明度
	m_BillboardArray.back().m_nBlendFlag = 1;         // ブレンドステートフラグ(0:通常描画　1:加算合成色描画)
	m_BillboardArray.back().m_nDrawFlag = 0;          // 描画フラグ(0:ビルボードのみ)
	SetSrc(&m_BillboardArray.back());                 // バーテックスバッファの作成


	// ------------------------------------------------------------------------------------------
	// オブジェクトの生成
	for (DWORD i = 0; i < EFFECT_MAX; i++)
	{
		m_pObjArray.push_back(new CEffectBillFireObj(this));  // m_pObjArrayにオブジェクトを生成する
	}
};

//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャクラスのデストラクタ	
//
//------------------------------------------------------------------------
CEffectBillFireProc::~CEffectBillFireProc()
{
	for (int i = 0; i < m_BillboardArray.size(); i++)
	{
		SAFE_RELEASE(m_BillboardArray[i].m_pTexture);
		SAFE_RELEASE(m_BillboardArray[i].m_pVertexBuffer);
	}
};

//------------------------------------------------------------------------
//
//	ビルボード構造体配列のアドレスを返す	
//
//  Bint idx ビルボード構造体配列の要素番号
//
//	戻り値 BILLBOARDBASE*　　配列のアドレス
//
//------------------------------------------------------------------------
BILLBOARDBASE* CEffectBillFireProc::GetBillArrayPtr(int idx)
{
	if (idx < 0 || idx >= m_BillboardArray.size())
	{
		MessageBox(nullptr, _T("■ GetBillArrayPtr( idx) ■"), _T("指定要素番号の配列がありません"), MB_OK);
		return nullptr;
	}
	return &m_BillboardArray[idx];
}


//------------------------------------------------------------------------
//
//	スプライトイメージの読み込み	
//
//  TCHAR* szFName            ビルボードのテクスチャファイル名
//  BILLBOARDBASE* pBillBase  ビルボード構造体アドレス
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CEffectBillFireProc::Load(TCHAR* szFName, BILLBOARDBASE* pBillBase)
{

	//テクスチャー読み込み	
	if (FAILED(GameDevice()->m_pD3D->CreateShaderResourceViewFromFile(szFName, &pBillBase->m_pTexture,
		pBillBase->m_dwImageWidth, pBillBase->m_dwImageHeight, 3)))
	{
		MessageBox(0, _T("ビルボード　テクスチャーを読み込めません"), szFName, MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャのバーティクスバッファ作成	
//	（ジオメトリ関連を初期化）	
//
//	BILLBOARDBASE* pBillBase
//
//	戻り値 HRESULT
//		S_OK	= 正常
//		E_FAIL	= 異常
//
//------------------------------------------------------------------------
HRESULT CEffectBillFireProc::SetSrc(BILLBOARDBASE* pBillBase)
{
	// ビルボード用の板ポリゴンを作成し、バーテックスバッファを作成する

	//バーテックスバッファー作成(裏向きの座標で作成)
	BILLBOARD_VERTEX vertices[] =
	{
		VECTOR3(-pBillBase->m_fDestWidth / 2,  pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)(pBillBase->m_dwSrcX + pBillBase->m_dwSrcWidth) / pBillBase->m_dwImageWidth, (float)pBillBase->m_dwSrcY / pBillBase->m_dwImageHeight),                     //頂点1  左上
		VECTOR3(pBillBase->m_fDestWidth / 2,  pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)pBillBase->m_dwSrcX / pBillBase->m_dwImageWidth,                (float)pBillBase->m_dwSrcY / pBillBase->m_dwImageHeight),                        //頂点2　右上
		VECTOR3(-pBillBase->m_fDestWidth / 2, -pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)(pBillBase->m_dwSrcX + pBillBase->m_dwSrcWidth) / pBillBase->m_dwImageWidth, (float)(pBillBase->m_dwSrcY + pBillBase->m_dwSrcHeight) / pBillBase->m_dwImageHeight),   //頂点3　左下
		VECTOR3(pBillBase->m_fDestWidth / 2, -pBillBase->m_fDestHeight / 2, 0), VECTOR2((float)pBillBase->m_dwSrcX / pBillBase->m_dwImageWidth,                (float)(pBillBase->m_dwSrcY + pBillBase->m_dwSrcHeight) / pBillBase->m_dwImageHeight),      //頂点4　右下
	};

	// ２度目以降に初期化されることの対策
	SAFE_RELEASE(pBillBase->m_pVertexBuffer);

	// バーテックスバッファー作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(BILLBOARD_VERTEX) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(GameDevice()->m_pD3D->m_pDevice->CreateBuffer(&bd, &InitData, &pBillBase->m_pVertexBuffer)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャの開始	
//
//	・ビルボード　炎オブジェクト配列の中から、空いているビルボードを探して開始する
//    なお、ビルボード構造体配列の要素番号０を開始する
//
//  VECTOR3 vPos          ビルボードの発生位置(表示中心位置)
//
//	戻り値 bool
//		true	= 正常
//		false	= 異常
//
//------------------------------------------------------------------------
bool CEffectBillFireProc::Start(VECTOR3 vPos)
{
	return Start(0, vPos);    //  ビルボード構造体配列の要素番号０を開始する
}
//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャの開始	
//
//	・ビルボード　炎オブジェクト配列の中から、空いているビルボードを探して開始する
//
//  int     nBillIdx      ビルボード構造体配列の要素番号
//  VECTOR3 vPos          ビルボードの発生位置(表示中心位置)
//
//	戻り値 bool
//		true	= 正常
//		false	= 異常
//
//------------------------------------------------------------------------
bool CEffectBillFireProc::Start(int  nBillIdx, VECTOR3 vPos)
{
	for (DWORD i = 0; i < m_pObjArray.size(); i++)
	{
		if (!m_pObjArray[i]->GetActive())
		{
			((CEffectBillFireObj*)m_pObjArray[i])->Start(nBillIdx, vPos);
			return true;
		}
	}
	return false;
};

//------------------------------------------------------------------------
//
//	ビルボード　炎プロシージャの開始	
//  (炎の制御をＷｅａｐｏｎ等の親が行うとき)
//
//	・ビルボード　炎オブジェクト配列の中から、空いているビルボード　炎を探して開始する
//
//  int     nBillIdx      ビルボード構造体配列の要素番号（省略値０）。
//
//	戻り値 CEffectBillFireObj*
//		発生できたとき　　ビルボード　炎オブジェクトアドレス
//		発生できないとき　nullptr
//
//------------------------------------------------------------------------
CEffectBillFireObj* CEffectBillFireProc::Start(int  nBillIdx)
{
	for (DWORD i = 0; i < m_pObjArray.size(); i++)
	{
		if (!m_pObjArray[i]->GetActive())
		{
			m_pObjArray[i]->SetActive(true);
			((CEffectBillFireObj*)(m_pObjArray[i]))->m_bAuto = false;  // 炎の制御をＷｅａｐｏｎ等の親が行う
			((CEffectBillFireObj*)(m_pObjArray[i]))->m_nBillIdx = nBillIdx;
			((CEffectBillFireObj*)(m_pObjArray[i]))->m_dwFrame = 0;
			return ((CEffectBillFireObj*)(m_pObjArray[i]));
		}
	}
	return nullptr;
};



// ============================================================================================

//------------------------------------------------------------------------
//
//	ビルボード　炎オブジェクトのコンストラクタ	
//
//  引数　なし
//
//------------------------------------------------------------------------
CEffectBillFireObj::CEffectBillFireObj(CBaseProc* pProc) : CBaseObj(pProc)
{
	m_pD3D = GameDevice()->m_pD3D;
	m_pShader = GameDevice()->m_pShader;

	m_vPos = VECTOR3(0, 0, 0);
	m_vUVOffset = VECTOR2(0, 0);
	m_dwFrame = 0;

	m_nBillIdx = 0;

	//	初期化
	Init();
}

//------------------------------------------------------------------------
//
//	ビルボード　炎オブジェクトのデストラクタ	
//
//------------------------------------------------------------------------
CEffectBillFireObj::~CEffectBillFireObj()
{
	;
}
//------------------------------------------------------------------------
//	ビルボード　炎オブジェクトの初期化関数
//
//  各種設定値の初期化
//
//  引数　　なし
//
//  戻り値　　　なし
//------------------------------------------------------------------------
void CEffectBillFireObj::Init()
{
	m_bActive = false;
	m_bAuto = false;
}

//------------------------------------------------------------------------
//	ビルボード　炎オブジェクトのビルボード構造体配列のアドレスを返す関数
//
//  引数　　なし
//
//  戻り値　　　BILLBOARDBASE*   配列のアドレス
//------------------------------------------------------------------------
BILLBOARDBASE* CEffectBillFireObj::GetBillArrayPtr()
{
	return  ((CEffectBillFireProc*)m_pProc)->GetBillArrayPtr(m_nBillIdx);
}

//------------------------------------------------------------------------
//
//	ビルボード　炎オブジェクトの開始	
//
//  int       nBillIdx ビルボード構造体配列の要素番号。
//  VECTOR3   vPos     表示位置（ビルボードの中心点位置を指定）
//
//	戻り値 bool
//		true      表示継続
//		false     表示終了
//
//------------------------------------------------------------------------
bool CEffectBillFireObj::Start(int nBillIdx, VECTOR3 vPos)
{
	if (m_bActive) return false;  // すでに開始中

	m_bActive = true;
	m_nBillIdx = nBillIdx;   // ビルボード構造体配列の要素番号
	m_bAuto = true;          // 自動モード
	m_dwFrame = 0;
	m_vPos = vPos;

	return true;
}

//------------------------------------------------------------------------
//
//	ビルボード　炎オブジェクトの更新	
//
//	戻り値 なし
//
//------------------------------------------------------------------------
void CEffectBillFireObj::Update()
{
	DWORD dwIdx;

	if (!m_bActive) return;

	m_dwFrame++;
	dwIdx = m_dwFrame / EFFECT_ANIM_SPEED;

	if (dwIdx >= GetBillArrayPtr()->m_dwNumX * GetBillArrayPtr()->m_dwNumY)	// パターンの最後に達したとき
	{
		if (m_bAuto) {  // 自動モードのときは終了
			m_bActive = false;
			return;
		}
		else {
			dwIdx = 0; // 炎の制御をＷｅａｐｏｎ等の親が行うときは、アニメーションを繰り返す
		}
	}

	// アニメーションのためのテクスチャオフセットを設定する
	m_vUVOffset.x = (float)(dwIdx % GetBillArrayPtr()->m_dwNumX * GetBillArrayPtr()->m_dwSrcWidth);   // Ｘ方向の変位
	m_vUVOffset.y = (float)(dwIdx / GetBillArrayPtr()->m_dwNumX * GetBillArrayPtr()->m_dwSrcHeight);  // Ｙ方向の変位
}
//------------------------------------------------------------------------
//
//	ビルボード　炎オブジェクトを画面にレンダリング	
//
//	戻り値 bool
//		true      表示継続
//		false     表示終了
//
//------------------------------------------------------------------------
void CEffectBillFireObj::Draw()
{

	if (!m_bActive) return;

	//ビルボードの、視点を向くワールドトランスフォームを求める
	MATRIX4X4 mWorld = GetLookatMatrix(m_vPos, GameDevice()->m_vEyePt);
	// 描画中心位置の移動をする
	MATRIX4X4 mPosUp = XMMatrixTranslation(GetBillArrayPtr()->m_fDestCenterX - GetBillArrayPtr()->m_fDestWidth / 2,
		GetBillArrayPtr()->m_fDestCenterY - GetBillArrayPtr()->m_fDestHeight / 2, 0);
	mWorld = mPosUp * mWorld;

	//使用するシェーダーのセット
	m_pD3D->m_pDeviceContext->VSSetShader(m_pShader->m_pEffect3D_VS_BILL, nullptr, 0);
	m_pD3D->m_pDeviceContext->PSSetShader(m_pShader->m_pEffect3D_PS, nullptr, 0);

	//バーテックスバッファーをセット
	UINT stride = sizeof(BILLBOARD_VERTEX);
	UINT offset = 0;
	ID3D11Buffer* pVertexBuffer = GetBillArrayPtr()->m_pVertexBuffer;   // バーテックスバッファ
	m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	UINT mask = 0xffffffff;
	if (GetBillArrayPtr()->m_nBlendFlag == 1)   // 加算合成色指定
	{
		// 加算合成色のブレンディングを設定
		m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateAdd, nullptr, mask);
	}

	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_EFFECT cb;
	ZeroMemory(&cb, sizeof(cb));

	if (SUCCEEDED(m_pD3D->m_pDeviceContext->Map(m_pShader->m_pConstantBufferEffect, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド、カメラ、射影行列、テクスチャーオフセットを渡す
		cb.mWVP = XMMatrixTranspose(mWorld * GameDevice()->m_mView * GameDevice()->m_mProj);

		cb.vUVOffset.x = m_vUVOffset.x / GetBillArrayPtr()->m_dwImageWidth;		// テクスチャアニメーションのオフセット
		cb.vUVOffset.y = m_vUVOffset.y / GetBillArrayPtr()->m_dwImageHeight;		// テクスチャアニメーションのオフセット
		cb.vUVScale.x = 1;
		cb.vUVScale.y = 1;
		cb.fAlpha = GetBillArrayPtr()->m_fAlpha;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pD3D->m_pDeviceContext->Unmap(m_pShader->m_pConstantBufferEffect, 0);
	}
	//このコンスタントバッファーをどのシェーダーで使うか
	m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferEffect);
	m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pShader->m_pConstantBufferEffect);
	//頂点インプットレイアウトをセット
	m_pD3D->m_pDeviceContext->IASetInputLayout(m_pShader->m_pEffect3D_VertexLayout_BILL);
	//プリミティブ・トポロジーをセット
	m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//テクスチャーをシェーダーに渡す
	m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &m_pD3D->m_pSampleLinear);
	m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &GetBillArrayPtr()->m_pTexture);
	//プリミティブをレンダリング
	m_pD3D->m_pDeviceContext->Draw(4, 0);

	if (GetBillArrayPtr()->m_nBlendFlag == 1)   // 加算合成色指定
	{
		// 通常のブレンディングに戻す
		m_pD3D->m_pDeviceContext->OMSetBlendState(m_pD3D->m_pBlendStateTrapen, nullptr, mask);
	}
}


