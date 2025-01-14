#include "shader_LIGHT.h"

namespace EPITOME
{

	Shader_LIGHT::Shader_LIGHT(WCHAR* filename)
	{
		m_vertexShader = 0;
		m_pixelShader = 0;
		m_layout = 0;
		m_sampleState = 0;
		m_matrixBuffer = 0;
		m_cameraBuffer = 0;
		m_lightBuffer = 0;

		this->m_filename = filename;

		bd.Add(new ID3D11Buffer**[3]{&m_matrixBuffer, &m_cameraBuffer, &m_lightBuffer}, 3);
	}

	Shader_LIGHT::Shader_LIGHT(const Shader_LIGHT& other)
	{
	}

	Shader_LIGHT::~Shader_LIGHT()
	{
	}

	bool Shader_LIGHT::Initialize(ID3D11Device* device, HWND hwnd)
	{
		bool result;


		// Initialize the vertex and pixel shaders.
		result = InitializeShader(device, hwnd, m_filename);
		if(!result)
		{
			return false;
		}

		return true;
	}

	void Shader_LIGHT::Shutdown()
	{
		// Shutdown the vertex and pixel shaders as well as the related objects.
		ShutdownShader();

		return;
	}

	bool Shader_LIGHT::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
		D3DXMATRIX projectionMatrix, Transform transform, Orientation orient)
	{
		bool result;
		
		// Set the shader parameters that it will use for rendering.
		result = PrepareShader(deviceContext, worldMatrix, viewMatrix, projectionMatrix, transform, orient);
		if(!result)
		{
			return false;
		}

		// Now render the prepared buffers with the shader.
		RenderShader(deviceContext, indexCount);

		return true;
	}




	bool Shader_LIGHT::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename)
	{
		HRESULT result;
		ID3D10Blob* errorMessage;
		ID3D10Blob* VSBuffer;
		ID3D10Blob* PSBuffer;
		D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
		unsigned int numElements;
		D3D11_SAMPLER_DESC samplerDesc;
		D3D11_BUFFER_DESC matrixBufferDesc;
		D3D11_BUFFER_DESC cameraBufferDesc;
		D3D11_BUFFER_DESC lightBufferDesc;


		// Initialize the pointers this function will use to null.
		errorMessage = 0;
		VSBuffer = 0;
		PSBuffer = 0;

		// Compile the vertex shader code.
		result = D3DX11CompileFromFile(filename, NULL, NULL, "VS", "vs_5_0", 0, 0, 0, 
										&VSBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				ThrowBlobError(errorMessage, hwnd, filename);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				DisplayMessage(hwnd, Error, false, false, L"Missing Shader File", filename);
			}

			return false;
		}

		// Compile the pixel shader code.
		result = D3DX11CompileFromFile(filename, NULL, NULL, "PS", "ps_5_0", 0, 0, 0, 
										&PSBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				ThrowBlobError(errorMessage, hwnd, filename);
			}
			// If there was nothing in the error message then it simply could not find the file itself.
			else
			{
				DisplayMessage(hwnd, Error, false, false, L"Missing Shader File", filename);
			}

			return false;
		}

		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(), NULL, &m_vertexShader);
		if(FAILED(result))
		{
			return false;
		}

		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(PSBuffer->GetBufferPointer(), PSBuffer->GetBufferSize(), NULL, &m_pixelShader);
		if(FAILED(result))
		{
			return false;
		}

		// Create the vertex input layout description.
		// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

		polygonLayout[2].SemanticName = "NORMAL";
		polygonLayout[2].SemanticIndex = 0;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[2].InputSlot = 0;
		polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[2].InstanceDataStepRate = 0;

		// Get a count of the elements in the layout.
		numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
		result = device->CreateInputLayout(polygonLayout, numElements, VSBuffer->GetBufferPointer(), VSBuffer->GetBufferSize(),
											&m_layout);
		if(FAILED(result))
		{
			return false;
		}

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
		VSBuffer->Release();
		VSBuffer = 0;

		PSBuffer->Release();
		PSBuffer = 0;

		// Create a texture sampler state description.
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Create the texture sampler state.
		result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
		if(FAILED(result))
		{
			return false;
		}

		// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
		if(FAILED(result))
		{
			return false;
		}

		// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
		cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
		cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cameraBufferDesc.MiscFlags = 0;
		cameraBufferDesc.StructureByteStride = 0;

		// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
		if(FAILED(result))
		{
			return false;
		}

		// Setup the description of the light dynamic constant buffer that is in the pixel shader.
		// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.ByteWidth = sizeof(LightBufferType);
		lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.MiscFlags = 0;
		lightBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
		if(FAILED(result))
		{
			return false;
		}

		return true;
	}


	void Shader_LIGHT::ShutdownShader()
	{
		bd.Shutdown();

		// Release the sampler state.
		if(m_sampleState)
		{
			m_sampleState->Release();
			m_sampleState = 0;
		}

		// Release the layout.
		if(m_layout)
		{
			m_layout->Release();
			m_layout = 0;
		}

		// Release the pixel shader.
		if(m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = 0;
		}

		// Release the vertex shader.
		if(m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = 0;
		}

	return;
	}

	bool Shader_LIGHT::PrepareShader(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
		D3DXMATRIX projectionMatrix, Transform transform, Orientation orient)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		unsigned int bufferNumber;
		MatrixBufferType* dataPtr;
		LightBufferType* dataPtr2;
		CameraBufferType* dataPtr3;

		D3DXQUATERNION quad;
		GetQuaternion(quad, transform.rotation.x, transform.rotation.y, transform.rotation.z, orient);
		D3DXMatrixTransformation(&worldMatrix, NULL, NULL, &D3DXVECTOR3(transform.scale.x, transform.scale.y, transform.scale.z),
			NULL, &quad, &D3DXVECTOR3(transform.translation.x, transform.translation.y, transform.translation.z));
		
		// Transpose the matrices to prepare them for the shader.
		D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
		D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
		D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		dataPtr = (MatrixBufferType*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_matrixBuffer, 0);

		// Set the position of the constant buffer in the vertex shader.
		bufferNumber = 0;

		// Now set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

		// Lock the camera constant buffer so it can be written to.
		result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		dataPtr3 = (CameraBufferType*)mappedResource.pData;

		// Copy the camera position into the constant buffer.
		dataPtr3->cameraPosition = r_cameraPosition;
		dataPtr3->padding = 0.0f;

		// Unlock the camera constant buffer.
		deviceContext->Unmap(m_cameraBuffer, 0);

		// Set the position of the camera constant buffer in the vertex shader.
		// (The first in the VS is the matrix buffer)
		bufferNumber = 1;

		// Now set the camera constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);	

		// Set shader texture resource in the pixel shader.
		deviceContext->PSSetShaderResources(0, 1, &r_texture);

		// Lock the light constant buffer so it can be written to.
		result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		dataPtr2 = (LightBufferType*)mappedResource.pData;

		// Copy the lighting variables into the constant buffer.
		dataPtr2->ambientColor = r_ambientColor;
		dataPtr2->diffuseColor = r_diffuseColor;
		dataPtr2->lightDirection = r_lightDirection;
		dataPtr2->specularColor = r_specularColor;
		dataPtr2->specularPower = r_specularPower;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_lightBuffer, 0);

		// Set the position of the light constant buffer in the pixel shader.
		bufferNumber = 0;

		// Finally set the light constant buffer in the pixel shader with the updated values.
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

		return true;
	}


	void Shader_LIGHT::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		// Set the vertex input layout.
		deviceContext->IASetInputLayout(m_layout);

		// Set the vertex and pixel shaders that will be used to render this triangle.
		deviceContext->VSSetShader(m_vertexShader, NULL, 0);
		deviceContext->PSSetShader(m_pixelShader, NULL, 0);

		// Set the sampler state in the pixel shader.
		deviceContext->PSSetSamplers(0, 1, &m_sampleState);

		// Render the triangle.
		deviceContext->DrawIndexed(indexCount, 0, 0);

		return;
	}

	void Shader_LIGHT::SetParameters(ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection,
								  D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 cameraPosition,
								  D3DXVECTOR4 specularColor, float specularPower)
	{
		r_texture = texture;
		r_lightDirection = lightDirection;
		r_ambientColor = ambientColor;
		r_diffuseColor = diffuseColor;
		r_cameraPosition = cameraPosition;
		r_specularColor = specularColor;
		r_specularPower = specularPower;
	}

	Shader* Shader_LIGHT::Clone()
    {
		Shader_LIGHT* i = new Shader_LIGHT(*this);
		i->r_ambientColor = this->r_ambientColor;
		i->r_cameraPosition = this->r_cameraPosition;
		i->r_diffuseColor = this->r_diffuseColor;
		i->r_lightDirection = this->r_lightDirection;
		i->r_specularColor = this->r_specularColor;
		i->r_specularPower = this->r_specularPower;
		i->r_texture = this->r_texture;
		i->m_layout = this->m_layout;
		i->m_matrixBuffer = this->m_matrixBuffer;
		i->m_pixelShader = this->m_pixelShader;
		i->m_sampleState = this->m_sampleState;
		i->m_vertexShader = this->m_vertexShader;
		i->m_cameraBuffer = this->m_cameraBuffer;
		i->m_lightBuffer = this->m_lightBuffer;
        return i;
    }

}