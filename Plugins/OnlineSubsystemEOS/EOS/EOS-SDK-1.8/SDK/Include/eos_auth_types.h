// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "eos_common.h"
#include "eos_connect_types.h"

#pragma pack(push, 8)

EXTERN_C typedef struct EOS_AuthHandle* EOS_HAuth;

/**
 * All possible types of login methods, availability depends on permissions granted to the client
 *
 * @see EOS_Auth_Login
 * @see EOS_Auth_Credentials
 */
EOS_ENUM(EOS_ELoginCredentialType,
	/** Email / Password. Use of this login type is restricted. */
	EOS_LCT_Password = 0,
	/** Exchange code, typically generated by launcher */
	EOS_LCT_ExchangeCode = 1,
	/**
	 * Long-lived access token that is stored on the local device to allow persisting a user login session over multiple runs of the application.
	 * When using this login type, if an existing access token is not found or it is invalid or otherwise expired, the error result EOS_EResult::EOS_InvalidAuth is returned.
	 *
	 * @note On Desktop and Mobile platforms, the persistent access token is automatically managed by the SDK that stores it in the keychain of the currently logged in user of the local device.
	 * On Console platforms, after a successful login using the EOS_LCT_DeviceCode login type,
	 * the persistent access token is retrieved using the EOS_Auth_CopyUserAuthToken API and
	 * stored by the application for the currently logged in user of the local device.
	 */
	EOS_LCT_PersistentAuth = 2,
	/**
	 * Console platforms only; not available on Desktop or Mobile.
	 *
	 * Initiates a PIN grant login flow that is used to login a local user to their Epic Account for the first time,
	 * and also whenever their locally persisted login credentials would have expired.
	 *
	 * @details The flow is as following:
	 * 1. Game initiates the user login flow by calling EOS_Auth_Login API with the EOS_LCT_DeviceCode login type.
	 * 2. The SDK internally requests the authentication backend service to begin the login flow, and returns the game
	 * a new randomly generated device code along with authorization URL information needed to complete the flow.
	 * This information is returned via the EOS_Auth_Login API callback. The EOS_Auth_LoginCallbackInfo::ResultCode
	 * will be set to EOS_Auth_PinGrantCode and the EOS_Auth_PinGrantInfo struct will contain the needed information.
	 * 3. Game presents the device code and the authorization URL information on screen to the end-user.
	 * 4. The user will login to their Epic Account using an external device, e.g. a mobile device or a desktop PC,
	 * by browsing to the presented authentication URL and entering the device code presented by the game on the console.
	 * 5. Once the user has successfully logged in on their external device, the SDK will call the EOS_Auth_Login callback
	 * once more with the operation result code. If the user failed to login within the allowed time before the device code
	 * would expire, the result code returned by the callback will contain the appropriate error result.
	 *
	 * @details After logging in a local user for the first time, the game can remember the user login to allow automatically logging
	 * in the same user the next time they start the game. This avoids prompting the same user to go through the login flow
	 * across multiple game sessions over long periods of time.
	 * To do this, after a successful login using the EOS_LCT_DeviceCode login type, the game can call the EOS_Auth_CopyUserAuthToken API
	 * to retrieve a long-lived refresh token that is specifically created for this purpose on Console. The game can store
	 * the long-lived refresh token locally on the device, for the currently logged in local user of the device.
	 * Then, on subsequent game starts the game can call the EOS_Auth_Login API with the previously stored refresh token and
	 * using the EOS_LCT_PersistentAuth login type to automatically login the current local user of the device.
	 */
	EOS_LCT_DeviceCode = 3,
	/** Request credentials from the EOS SDK Dev Auth Tool */
	EOS_LCT_Developer = 4,
	/** Refresh token */
	EOS_LCT_RefreshToken = 5,
	/** Initiate a login through the account portal. Cannot be used on consoles. */
	EOS_LCT_AccountPortal = 6,
	/**
	 * Initiate a login through an external provider, such as Steam, PSN, XBL, or Nintendo.
	 *
	 * @details EOS_InvalidUser will be returned when the external auth login fails due to an external account not being linked.
	 * An EOS_ContinuanceToken will be set in the EOS_Auth_LoginCallbackInfo data.
	 * EOS_Auth_LinkAccount should be called with the EOS_ContinuanceToken to continue the external account login and link the external account.
	 */
	EOS_LCT_ExternalAuth = 7
);

/** The most recent version of the EOS_Auth_Token struct. */
#define EOS_AUTH_TOKEN_API_LATEST 2

/**
 * Types of auth tokens
 *
 * @see EOS_Auth_CopyUserAuthToken
 * @see EOS_Auth_Token
 */
EOS_ENUM(EOS_EAuthTokenType,
	/** Auth token is for a validated client */
	EOS_ATT_Client = 0,
	/** Auth token is for a validated user */
	EOS_ATT_User = 1
);

/** A structure that contains an auth token. These structures are created by EOS_Auth_CopyUserAuthToken and must be passed to EOS_Auth_Token_Release. */
EOS_STRUCT(EOS_Auth_Token, (
	/** API Version: Set this to EOS_AUTH_TOKEN_API_LATEST. */
	int32_t ApiVersion;
	/** Name of the app related to the client ID involved with this token */
	const char* App;
	/** Client ID that requested this token */
	const char* ClientId;
	/** The Epic Online Services Account ID associated with this auth token */
	EOS_EpicAccountId AccountId;
	/** Access token for the current user login session */
	const char* AccessToken;
	/** Time before the access token expires, in seconds, relative to the call to EOS_Auth_CopyUserAuthToken */
	double ExpiresIn;
	/** Absolute time in UTC before the access token expires, in ISO 8601 format */
	const char* ExpiresAt;
	/** Type of auth token */
	EOS_EAuthTokenType AuthType;
	/**
	 * Refresh token.
	 *
	 * @details This member has two separate use cases:
	 * 1. Launching the game from Epic Games Launcher, having an intermediate process (such as another launcher) that uses
	 * Epic authentication in-between.
	 * In this use case, an intermediate launcher that has logged in the user using the EOS_LCT_ExchangeCode login type
	 * can copy the refresh token using the EOS_Auth_CopyUserAuthToken API and pass it in launch parameters for the started
	 * game client that can then login the user again using the refresh token this time with the EOS_LCT_RefreshToken
	 * login type.
	 * 2. Persistent auth on Console platforms, to remember user login across game sessions over longer period of times
	 * without having to prompt the user to explicitly login each time they run the game.
	 * In this use case, a local user is logged in with their Epic Account for the first time using the EOS_LCT_DeviceCode
	 * login type. Specific to the EOS_LCT_DeviceCode login type, the SDK receives a long-lived refresh token instead of a
	 * regular short-lived refresh token. The game can use the EOS_Auth_CopyUserAuthToken API to copy the long-lived refresh
	 * token and store it locally for the currently logged in local user of the console device.
	 */
	const char* RefreshToken;
	/** Time before the access token expires, in seconds, relative to the call to EOS_Auth_CopyUserAuthToken */
	double RefreshExpiresIn;
	/** Absolute time in UTC before the refresh token expires, in ISO 8601 format */
	const char* RefreshExpiresAt;
));

/**
 * Release the memory associated with an EOS_Auth_Token structure. This must be called on data retrieved from EOS_Auth_CopyUserAuthToken.
 *
 * @param AuthToken - The auth token structure to be released
 *
 * @see EOS_Auth_Token
 * @see EOS_Auth_CopyUserAuthToken
 */
EOS_DECLARE_FUNC(void) EOS_Auth_Token_Release(EOS_Auth_Token* AuthToken);

/** The most recent version of the EOS_Auth_Credentials struct. */
#define EOS_AUTH_CREDENTIALS_API_LATEST 3

/**
 * A structure that contains login credentials. What is required is dependent on the type of login being initiated.
 * 
 * This is part of the input structure EOS_Auth_LoginOptions and related to device auth.
 *
 * Use of the ID and Token fields differs based on the Type. They should be null, unless specified:
 * EOS_LCT_Password - ID is the email address, and Token is the password.
 * EOS_LCT_ExchangeCode - Token is the exchange code.
 * EOS_LCT_PersistentAuth - If targeting console platforms, Token is the long lived refresh token. Otherwise N/A.
 * EOS_LCT_DeviceCode - N/A.
 * EOS_LCT_Developer - ID is the host (e.g. 127.0.0.1:10000), and Token is the credential name.
 * EOS_LCT_RefreshToken - Token is the refresh token.
 * EOS_LCT_AccountPortal - SystemAuthCredentialsOptions may be required if targeting mobile platforms. Otherwise N/A.
 * EOS_LCT_ExternalAuth - Token is the external auth token specified by ExternalType.
 *
 * @see EOS_ELoginCredentialType
 * @see EOS_Auth_Login
 * @see EOS_Auth_DeletePersistentAuthOptions
 */ 
EOS_STRUCT(EOS_Auth_Credentials, (
	/** API Version: Set this to EOS_AUTH_CREDENTIALS_API_LATEST. */
	int32_t ApiVersion;
	/** ID of the user logging in, based on EOS_ELoginCredentialType */
	const char* Id;
	/** Credentials or token related to the user logging in */
	const char* Token;
	/** Type of login. Needed to identify the auth method to use */
	EOS_ELoginCredentialType Type;
	/** 
	 * This field is for system specific options, if any.
	 *
	 * If provided, the structure will be located in (System)/eos_(system).h.
	 * The structure will be named EOS_(System)_Auth_CredentialsOptions.
	 */
	void* SystemAuthCredentialsOptions;
	/**
	 * Type of external login. Needed to identify the external auth method to use.
	 * Used when login type is set to EOS_LCT_ExternalAuth, ignored for other EOS_ELoginCredentialType methods.
	 */
	EOS_EExternalCredentialType ExternalType;
));

/** The most recent version of the EOS_Auth_PinGrantInfo struct. */
#define EOS_AUTH_PINGRANTINFO_API_LATEST 2

/**
 * Intermediate data needed to complete the EOS_LCT_DeviceCode login flow, returned by EOS_Auth_LoginCallbackInfo.  
 * The data inside should be exposed to the user for entry on a secondary device
 * All data must be copied out before the completion of this callback
 */
EOS_STRUCT(EOS_Auth_PinGrantInfo, (
	/** API Version: Set this to EOS_AUTH_PINGRANTINFO_API_LATEST. */
	int32_t ApiVersion;
	/** Code the user must input on an external device to activate the login */
	const char* UserCode;
	/** The end-user verification URI. Users can be asked to manually type this into their browser. */
	const char* VerificationURI;
	/** Time the user has, in seconds, to complete the process or else timeout */
	int32_t ExpiresIn;
	/** A verification URI that includes the user code. Useful for non-textual transmission. */
	const char* VerificationURIComplete;
));

/** The most recent version of the EOS_Auth_AccountFeatureRestrictedInfo struct. */
#define EOS_AUTH_ACCOUNTFEATURERESTRICTEDINFO_API_LATEST 1

/**
 * Intermediate data needed to complete account restriction verification during login flow, returned by EOS_Auth_LoginCallbackInfo when the ResultCode is EOS_Auth_AccountFeatureRestricted
 * The URI inside should be exposed to the user for entry in a web browser. The URI must be copied out of this struct before completion of the callback.
 */
EOS_STRUCT(EOS_Auth_AccountFeatureRestrictedInfo, (
	/** API Version: Set this to EOS_AUTH_ACCOUNTFEATURERESTRICTEDINFO_API_LATEST. */
	int32_t ApiVersion;
	/** The end-user verification URI. Users must be asked to open the page in a browser to address the restrictions */
	const char* VerificationURI;
));

/* Flags that describe user permissions */
EOS_ENUM(EOS_EAuthScopeFlags,
	EOS_AS_NoFlags = 0x0,
	/** Permissions to see your account ID, display name, language and country */
	EOS_AS_BasicProfile = 0x1,
	/** Permissions to see a list of your friends who use this application */
	EOS_AS_FriendsList = 0x2,
	/** Permissions to set your online presence and see presence of your friends */
	EOS_AS_Presence = 0x4,
	/** Permissions to manage the Epic friends list. This scope is restricted to Epic first party products, and attempting to use it will result in authentication failures. */
	EOS_AS_FriendsManagement = 0x8
);

/** A set of bitwise operators provided when the enum is provided as an `enum class`. */
#if defined(__cplusplus) && (__cplusplus >= 201103L || (defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
inline constexpr EOS_EAuthScopeFlags operator|(EOS_EAuthScopeFlags Left, EOS_EAuthScopeFlags Right) { return static_cast<EOS_EAuthScopeFlags>((__underlying_type(EOS_EAuthScopeFlags))Left | (__underlying_type(EOS_EAuthScopeFlags))Right); }
inline constexpr EOS_EAuthScopeFlags operator&(EOS_EAuthScopeFlags Left, EOS_EAuthScopeFlags Right) { return static_cast<EOS_EAuthScopeFlags>((__underlying_type(EOS_EAuthScopeFlags))Left & (__underlying_type(EOS_EAuthScopeFlags))Right); }
inline constexpr EOS_EAuthScopeFlags operator^(EOS_EAuthScopeFlags Left, EOS_EAuthScopeFlags Right) { return static_cast<EOS_EAuthScopeFlags>((__underlying_type(EOS_EAuthScopeFlags))Left ^ (__underlying_type(EOS_EAuthScopeFlags))Right); }
inline constexpr EOS_EAuthScopeFlags& operator|=(EOS_EAuthScopeFlags& Left, EOS_EAuthScopeFlags Right) { return Left = Left | Right; }
inline constexpr EOS_EAuthScopeFlags& operator&=(EOS_EAuthScopeFlags& Left, EOS_EAuthScopeFlags Right) { return Left = Left & Right; }
inline constexpr EOS_EAuthScopeFlags& operator^=(EOS_EAuthScopeFlags& Left, EOS_EAuthScopeFlags Right) { return Left = Left ^ Right; }
#endif

/** The most recent version of the EOS_Auth_Login API. */
#define EOS_AUTH_LOGIN_API_LATEST 2

/**
 * Input parameters for the EOS_Auth_Login function.
 */
EOS_STRUCT(EOS_Auth_LoginOptions, (
	/** API Version: Set this to EOS_AUTH_LOGIN_API_LATEST. */
	int32_t ApiVersion;
	/** Credentials specified for a given login method */
	const EOS_Auth_Credentials* Credentials;
	/** Auth scope flags are permissions to request from the user while they are logging in. This is a bitwise-or union of EOS_EAuthScopeFlags flags defined above */
	EOS_EAuthScopeFlags ScopeFlags;
));

/**
 * Output parameters for the EOS_Auth_Login Function.
 */
EOS_STRUCT(EOS_Auth_LoginCallbackInfo, (
	/** The EOS_EResult code for the operation. EOS_Success indicates that the operation succeeded; other codes indicate errors. */
	EOS_EResult ResultCode;
	/** Context that was passed into EOS_Auth_Login */
	void* ClientData;
	/** The Epic Online Services Account ID of the local user who has logged in */
	EOS_EpicAccountId LocalUserId;
	/** Optional data returned in the middle of a EOS_LCT_DeviceCode request */
	const EOS_Auth_PinGrantInfo* PinGrantInfo;
	/** If the user was not found with external auth credentials passed into EOS_Auth_Login, this continuance token can be passed to EOS_Auth_LinkAccount to continue the flow. */
	EOS_ContinuanceToken ContinuanceToken;
	/** If the user trying to login is restricted from doing so, the ResultCode of this structure will be EOS_Auth_AccountFeatureRestricted, and AccountFeatureRestrictedInfo will be populated with the data needed to get past the restriction */
	const EOS_Auth_AccountFeatureRestrictedInfo* AccountFeatureRestrictedInfo;
));

/**
 * Function prototype definition for callbacks passed to EOS_Auth_Login
 * @param Data A EOS_Auth_LoginCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnLoginCallback, const EOS_Auth_LoginCallbackInfo* Data);

/** The most recent version of the EOS_Auth_Logout API. */
#define EOS_AUTH_LOGOUT_API_LATEST 1

/**
 * Input parameters for the EOS_Auth_Logout function.
 */
EOS_STRUCT(EOS_Auth_LogoutOptions, (
	/** API Version: Set this to EOS_AUTH_LOGOUT_API_LATEST. */
	int32_t ApiVersion;
	/** The Epic Online Services Account ID of the local user who is being logged out */
	EOS_EpicAccountId LocalUserId;
));

/**
 * Output parameters for the EOS_Auth_Logout Function.
 */
EOS_STRUCT(EOS_Auth_LogoutCallbackInfo, (
	/** The EOS_EResult code for the operation. EOS_Success indicates that the operation succeeded; other codes indicate errors. */
	EOS_EResult ResultCode;
	/** Context that was passed into EOS_Auth_Login */
	void* ClientData;
	/** The Epic Online Services Account ID of the local user requesting the information */
	EOS_EpicAccountId LocalUserId;
));

/**
 * Function prototype definition for callbacks passed to EOS_Auth_Logout
 * @param Data A EOS_Auth_LogoutCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnLogoutCallback, const EOS_Auth_LogoutCallbackInfo* Data);

/**
 * Flags used to describe how the account linking operation is to be performed.
 *
 * @see EOS_Auth_LinkAccount
 */
EOS_ENUM(EOS_ELinkAccountFlags,
	/**
	 * Default flag used for a standard account linking operation.
	 *
	 * This flag is set when using a continuance token received from a previous call to the EOS_Auth_Login API,
	 * when the local user has not yet been successfully logged in to an Epic Account yet.
	 */
	EOS_LA_NoFlags = 0x0,
	/**
	 * Specified when the EOS_ContinuanceToken describes a Nintendo NSA ID account type.
	 * 
	 * This flag is used only with, and must be set, when the continuance token was received from a previous call
	 * to the EOS_Auth_Login API using the EOS_EExternalCredentialType::EOS_ECT_NINTENDO_NSA_ID_TOKEN login type.
	 */
	EOS_LA_NintendoNsaId = 0x1
);

/** The most recent version of the EOS_Auth_LinkAccount API. */
#define EOS_AUTH_LINKACCOUNT_API_LATEST 1

/**
 * Input parameters for the EOS_Auth_LinkAccount function.
 */
EOS_STRUCT(EOS_Auth_LinkAccountOptions, (
	/** API Version: Set this to EOS_AUTH_LINKACCOUNT_API_LATEST. */
	int32_t ApiVersion;
	/**
	 * Combination of the enumeration flags to specify how the account linking operation will be performed.
	 */
	EOS_ELinkAccountFlags LinkAccountFlags;
	/** 
	 * Continuance token received from a previous call to the EOS_Auth_Login API.
	 *
	 * A continuance token is received in the case when the external account used for login was not found to be linked
	 * against any existing Epic Account. In such case, the application needs to proceed with an account linking operation in which case
	 * the user is first asked to create a new account or login into their existing Epic Account, and then link their external account to it.
	 * Alternatively, the application may suggest the user to login using another external account that they have already linked to their existing Epic Account.
	 * In this flow, the external account is typically the currently logged in local platform user account.
	 * It can also be another external user account that the user is offered to login with.
	 */
	EOS_ContinuanceToken ContinuanceToken;
	
	/**
	 * The Epic Online Services Account ID of the logged in local user whose Epic Account will be linked with the local Nintendo NSA ID Account. By default set to NULL.
	 *
	 * This parameter is only used and required to be set when EOS_ELinkAccountFlags::EOS_LA_NintendoNsaId is specified.
	 * Otherwise, set to NULL, as the standard account linking and login flow using continuance token will handle logging in the user to their Epic Account.
	 */
	EOS_EpicAccountId LocalUserId;
));

/**
 * Output parameters for the EOS_Auth_LinkAccount Function.
 */
EOS_STRUCT(EOS_Auth_LinkAccountCallbackInfo, (
	/** The EOS_EResult code for the operation. EOS_Success indicates that the operation succeeded; other codes indicate errors. */
	EOS_EResult ResultCode;
	/** Context that was passed into EOS_Auth_LinkAccount */
	void* ClientData;
	/** The Epic Online Services Account ID of the local user whose account has been linked during login */
	EOS_EpicAccountId LocalUserId;
	/**
	 * Optional data returned when ResultCode is EOS_Auth_PinGrantCode.
	 *
	 * Once the user has logged in with their Epic Online Services account, the account will be linked with the external account supplied when EOS_Auth_Login was called.
	 * EOS_Auth_OnLinkAccountCallback will be fired again with ResultCode in EOS_Auth_LinkAccountCallbackInfo set to EOS_Success.
	 */
	const EOS_Auth_PinGrantInfo* PinGrantInfo;
));

/**
 * Function prototype definition for callbacks passed to EOS_Auth_LinkAccount
 * @param Data A EOS_Auth_LinkAccountCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnLinkAccountCallback, const EOS_Auth_LinkAccountCallbackInfo* Data);


/** The most recent version of the EOS_Auth_VerifyUserAuth API. */
#define EOS_AUTH_VERIFYUSERAUTH_API_LATEST 1

/**
 * Input parameters for the EOS_Auth_VerifyUserAuth function.
 * This operation is destructive, the pointer will remain the same but the data pointers inside will update
 */
EOS_STRUCT(EOS_Auth_VerifyUserAuthOptions, (
	/** API Version: Set this to EOS_AUTH_VERIFYUSERAUTH_API_LATEST. */
	int32_t ApiVersion;
	/** Auth token to verify against the backend service */
	const EOS_Auth_Token* AuthToken;
));

/**
 * Output parameters for the EOS_Auth_VerifyUserAuth Function.
 */
EOS_STRUCT(EOS_Auth_VerifyUserAuthCallbackInfo, (
	/** The EOS_EResult code for the operation. EOS_Success indicates that the operation succeeded; other codes indicate errors. */
	EOS_EResult ResultCode;
	/** Context that was passed into EOS_Auth_Login */
	void* ClientData;
));

/**
 * Function prototype definition for callbacks passed to EOS_Auth_VerifyUserAuth
 * @param Data A EOS_Auth_VerifyUserAuthCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnVerifyUserAuthCallback, const EOS_Auth_VerifyUserAuthCallbackInfo* Data);


/** The most recent version of the EOS_Auth_CopyUserAuthToken API. */
#define EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST 1

/**
 * Input parameters for the EOS_Auth_CopyUserAuthToken function.
 */
EOS_STRUCT(EOS_Auth_CopyUserAuthTokenOptions, (
	/** API Version: Set this to EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST. */
	int32_t ApiVersion;
));


/** The most recent version of the EOS_Auth_AddNotifyLoginStatusChanged API. */
#define EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST 1

/**
 * Input parameters for the EOS_Auth_AddNotifyLoginStatusChanged Function.
 */
EOS_STRUCT(EOS_Auth_AddNotifyLoginStatusChangedOptions, (
	/** API Version: Set this to EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST. */
	int32_t ApiVersion;
));


/** The most recent version of the EOS_Auth_DeletePersistentAuth API. */
#define EOS_AUTH_DELETEPERSISTENTAUTH_API_LATEST 2

/**
 * Input parameters for the EOS_Auth_DeletePersistentAuth function.
 */
EOS_STRUCT(EOS_Auth_DeletePersistentAuthOptions, (
	/** API Version: Set this to EOS_AUTH_DELETEPERSISTENTAUTH_API_LATEST. */
	int32_t ApiVersion;
	/**
	 * A long-lived refresh token that is used with the EOS_LCT_PersistentAuth login type and is to be revoked from the authentication server. Only used on Console platforms.
	 * On Desktop and Mobile platforms, set this parameter to NULL.
	 */
	const char* RefreshToken;
));

/**
 * Output parameters for the EOS_Auth_DeletePersistentAuth Function.
 */
EOS_STRUCT(EOS_Auth_DeletePersistentAuthCallbackInfo, (
	/** The EOS_EResult code for the operation. EOS_Success indicates that the operation succeeded; other codes indicate errors. */
	EOS_EResult ResultCode;
	/** Context that was passed into EOS_Auth_DeletePersistentAuth */
	void* ClientData;
));

/**
 * Function prototype definition for callbacks passed to EOS_Auth_DeletePersistentAuth
 * @param Data A EOS_Auth_DeletePersistentAuthCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnDeletePersistentAuthCallback, const EOS_Auth_DeletePersistentAuthCallbackInfo* Data);


/**
 * Output parameters for the EOS_Auth_OnLoginStatusChangedCallback Function.
 */
EOS_STRUCT(EOS_Auth_LoginStatusChangedCallbackInfo, (
	/** Context that was passed into EOS_Auth_AddNotifyLoginStatusChanged */
	void* ClientData;
	/** The Epic Online Services Account ID of the local user whose status has changed */
	EOS_EpicAccountId LocalUserId;
	/** The status prior to the change */
	EOS_ELoginStatus PrevStatus;
	/** The status at the time of the notification */
	EOS_ELoginStatus CurrentStatus;
));

/**
 * Function prototype definition for notifications that come from EOS_Auth_AddNotifyLoginStatusChanged
 *
 * @param Data A EOS_Auth_LoginStatusChangedCallbackInfo containing the output information and result
 */
EOS_DECLARE_CALLBACK(EOS_Auth_OnLoginStatusChangedCallback, const EOS_Auth_LoginStatusChangedCallbackInfo* Data);

#pragma pack(pop)
