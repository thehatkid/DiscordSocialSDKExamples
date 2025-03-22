#include <string>
#include <iostream>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <csignal>

#include <discordpp.h>

// Replace with your Discord Application ID
#define APP_ID 1234567890123456789

std::shared_ptr<discordpp::Client> client;

std::atomic<bool> running = true;

void signalHandler(int signum)
{
	std::cout << "Shutting down..." << std::endl;

	running.store(false);
}

void clientLogCallback(std::string message, discordpp::LoggingSeverity severity)
{
	std::clog << "[Client][" << EnumToString(severity) << "] " << message;
}

void clientStatusChangedCallback(discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail)
{
	std::cout << "[Client Status] Changed to " << discordpp::Client::StatusToString(status) << std::endl;

	switch (status)
	{
		case discordpp::Client::Status::Ready:
		{
			std::cout << "[!] Client is Ready!" << std::endl;

			discordpp::UserHandle me = client->GetCurrentUser();

			std::cout << "[!] Logged in as " << me.Username() << " (ID: " << me.Id() << ")" << std::endl;
			std::cout << "[!] Friends count: " << client->GetRelationships().size() << std::endl;

			// Set Rich Presence

			// NOTE: You should change Party ID and Join Secret depending on your game state.
			// These are provided as example of joinable activity invite.

			discordpp::ActivityParty activityParty;
			activityParty.SetId("party_aac0ffee");
			activityParty.SetCurrentSize(2);
			activityParty.SetMaxSize(4);

			discordpp::ActivitySecrets secrets;
			secrets.SetJoin("L33tTheGameAndSomeSecretGoesHere");

			discordpp::Activity activity;
			activity.SetType(discordpp::ActivityTypes::Playing);
			activity.SetState("Enjoying with examples");
			activity.SetDetails("Very excited");
			activity.SetParty(activityParty);
			activity.SetSecrets(secrets);
			activity.SetSupportedPlatforms(discordpp::ActivityGamePlatforms::Desktop);

			client->UpdateRichPresence(activity, [](discordpp::ClientResult result)
			{
				if (result.Successful())
					std::cout << "Rich Presence has been updated successfully" << std::endl;
				else
					std::cerr << "Rich Presence update failed: " << result.ToString() << std::endl;
			});

			break;
		}

		default:
			if (error != discordpp::Client::Error::None)
			{
				std::cerr << "[Client] Got Error: " << discordpp::Client::ErrorToString(error) << " (" << errorDetail << ")" << std::endl;
				running.store(false);
			}
			break;
	}
}

void clientTokenExchangeCallback(
	discordpp::ClientResult result,
	std::string accessToken,
	std::string refreshToken,
	discordpp::AuthorizationTokenType tokenType,
	int32_t expiresIn,
	std::string scopes
)
{
	std::cout << "Got Access Token" << std::endl;

	client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [](discordpp::ClientResult result) 
	{
		if (result.Successful())
			client->Connect();
		else
			std::cerr << "Failure to update client token" << std::endl;
	});
}

void clientActivityInviteCreatedCallback(discordpp::ActivityInvite invite)
{
	std::cout << "Received Activity Invite from User " << invite.SenderId() << std::endl;

	if (std::optional<discordpp::MessageHandle> message = client->GetMessageHandle(invite.MessageId()))
		std::cout << "Activity Invite message: " << message->Content() << std::endl;

	// Accept invitation
	client->AcceptActivityInvite(invite, [](discordpp::ClientResult result, std::string joinSecret)
	{
		if (result.Successful()) {
			std::cout << "Activity Invite accepted successfully!" << std::endl;

			// Here you can create/join the lobby
			/*
			client->CreateOrJoinLobby(joinSecret, [=](discordpp::ClientResult result, uint64_t lobbyId)
			{
				if (result.Successful())
					std::cout << "Lobby joined successfully! (" << lobbyId << ")" << std::endl;
				else
					std::cerr << "Lobby join failed!" << std::endl;
			});
			*/
		} else {
			std::cerr << "Activity Invite accept failed!" << std::endl;
		}
	});
}

void clientActivityJoinCallback(std::string joinSecret)
{
	std::cout << "Activity Join" << std::endl;

	// Use "joinSecret" to connect the players in your game
}

int main()
{
	// Signal handler for stopping SDK callbacks loop
	std::signal(SIGINT, signalHandler);

	// Put your Access Token for quick authentication
	std::string accessToken = "";

	// Output linked SDK version
	std::cout << "Discord Social SDK version "
		<< discordpp::Client::GetVersionMajor() << "."
		<< discordpp::Client::GetVersionMinor() << "."
		<< discordpp::Client::GetVersionPatch()
		<< " (" << discordpp::Client::GetVersionHash() << ")" << std::endl;

	client = std::make_shared<discordpp::Client>();

	// Set up client callbacks
	client->AddLogCallback(clientLogCallback, discordpp::LoggingSeverity::Info);
	client->SetStatusChangedCallback(clientStatusChangedCallback);
	client->SetActivityInviteCreatedCallback(clientActivityInviteCreatedCallback);
	client->SetActivityJoinCallback(clientActivityJoinCallback);

	if (accessToken.empty())
	{
		// Authorize application with OAuth2
		discordpp::AuthorizationCodeVerifier codeVerifier = client->CreateAuthorizationCodeVerifier();

		discordpp::AuthorizationArgs authArgs {};
		authArgs.SetClientId(APP_ID);
		authArgs.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
		authArgs.SetCodeChallenge(codeVerifier.Challenge());

		client->Authorize(authArgs, [codeVerifier](discordpp::ClientResult result, std::string code, std::string redirectUri)
		{
			if (!result.Successful())
			{
				std::cerr << "Authorization failed: " << result.Error() << std::endl;
				running.store(false);
				return;
			}

			// Exchange Authorization Code for Access Token
			client->GetToken(APP_ID, code, codeVerifier.Verifier(), redirectUri, clientTokenExchangeCallback);
		});
	} else {
		client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [](discordpp::ClientResult result) 
		{
			if (result.Successful()) {
				client->Connect();
			} else {
				std::cerr << "Failure to update client token" << std::endl;
				running.store(false);
			}
		});
	}

	// Keep running to allow SDK to receive events and callbacks
	while (running.load())
	{
		discordpp::RunCallbacks();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	if (client->GetStatus() != discordpp::Client::Status::Connected)
	{
		client->Disconnect();

		// Wait for graceful SDK shutdown
		while (client->GetStatus() != discordpp::Client::Status::Disconnected)
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	std::cout << "Goodbye!" << std::endl;

	return 0;
}
