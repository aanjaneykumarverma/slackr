send_table <- function(header, dataframe, channel, ts = NULL) {
  slack_api_token <- get_slack_token()
  
  mrkdwn_table <- pandoc.table.return(dataframe,
                                      justify = 'left',
                                      style = 'rmarkdown')
  
  body_text <- glue("```{mrkdwn_table}```")
  
  response <- POST(
    url = "https://slack.com/api/chat.postMessage",
    body = list(
      blocks = list(
        list(
          type = "section",
          text = list(
            type = "mrkdwn",
            text = header
          )
        ),
        list(
          type = "section",
          text = list(
            type = "mrkdwn",
            text = body_text
          )
        )
      ),
      link_names = TRUE,
      slack_token = slack_api_token,
      channel = channel,
      thread_ts = ts
    ),
    add_headers(Authorization = glue("Bearer {slack_api_token}")),
    encode = "json"
  )
  
  return(content(response)$ts)
}